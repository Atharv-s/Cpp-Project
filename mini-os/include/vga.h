#ifndef VGA_H
#define VGA_H

#include "kernel.h"

enum vga_color {
    VGA_BLACK = 0, VGA_BLUE = 1, VGA_GREEN = 2, VGA_CYAN = 3,
    VGA_RED = 4, VGA_MAGENTA = 5, VGA_BROWN = 6, VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8, VGA_LIGHT_BLUE = 9, VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11, VGA_LIGHT_RED = 12, VGA_LIGHT_MAGENTA = 13,
    VGA_LIGHT_BROWN = 14, VGA_WHITE = 15,
};

namespace terminal {
    void initialize();
    void setcolor(uint8_t color);
    void putchar(char c);
    void write(const char* data);
    void write_dec(uint32_t n);
    void write_hex(uint32_t n);
    void clear();
    void backspace();
}

#endif
