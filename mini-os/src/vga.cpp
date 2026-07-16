#include "vga.h"

namespace {
    static const size_t VGA_WIDTH = 80;
    static const size_t VGA_HEIGHT = 25;

    size_t row;
    size_t column;
    uint8_t color;
    uint16_t* buffer;

    inline uint8_t vga_entry_color(vga_color fg, vga_color bg) {
        return fg | bg << 4;
    }
    inline uint16_t vga_entry(unsigned char c, uint8_t color) {
        return (uint16_t) c | (uint16_t) color << 8;
    }

    void scroll() {
        if (row < VGA_HEIGHT) return;
        for (size_t y = 1; y < VGA_HEIGHT; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                buffer[(y - 1) * VGA_WIDTH + x] = buffer[y * VGA_WIDTH + x];
            }
        }
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', color);
        }
        row = VGA_HEIGHT - 1;
    }

    void update_cursor() {
        uint16_t pos = row * VGA_WIDTH + column;
        outb(0x3D4, 0x0F);
        outb(0x3D5, (uint8_t)(pos & 0xFF));
        outb(0x3D4, 0x0E);
        outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
    }
}

namespace terminal {

void initialize() {
    row = 0;
    column = 0;
    color = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);
    buffer = (uint16_t*) 0xB8000;
    clear();
}

void clear() {
    for (size_t y = 0; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            buffer[y * VGA_WIDTH + x] = vga_entry(' ', color);
    row = 0;
    column = 0;
    update_cursor();
}

void setcolor(uint8_t c) {
    color = c;
}

void putchar(char c) {
    if (c == '\n') {
        column = 0;
        row++;
        scroll();
        update_cursor();
        return;
    }
    if (c == '\r') {
        column = 0;
        update_cursor();
        return;
    }
    buffer[row * VGA_WIDTH + column] = vga_entry((unsigned char)c, color);
    if (++column == VGA_WIDTH) {
        column = 0;
        row++;
        scroll();
    }
    update_cursor();
}

void backspace() {
    if (column == 0) {
        if (row == 0) return;
        row--;
        column = VGA_WIDTH - 1;
    } else {
        column--;
    }
    buffer[row * VGA_WIDTH + column] = vga_entry(' ', color);
    update_cursor();
}

void write(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++)
        putchar(data[i]);
}

void write_dec(uint32_t n) {
    if (n == 0) { putchar('0'); return; }
    char buf[12];
    int i = 0;
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (i > 0) putchar(buf[--i]);
}

void write_hex(uint32_t n) {
    const char* hex = "0123456789ABCDEF";
    write("0x");
    for (int i = 28; i >= 0; i -= 4)
        putchar(hex[(n >> i) & 0xF]);
}

} // namespace terminal
