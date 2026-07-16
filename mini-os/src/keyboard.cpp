#include "keyboard.h"
#include "idt.h"

namespace {
    const size_t BUF_SIZE = 256;
    char buffer[BUF_SIZE];
    volatile size_t head = 0, tail = 0;
    bool shift_pressed = false;

    // US QWERTY scancode set 1 -> ASCII (unshifted / shifted)
    const char scancode_ascii[128] = {
        0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
        '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
        0 /*ctrl*/, 'a','s','d','f','g','h','j','k','l',';','\'','`',
        0 /*lshift*/, '\\','z','x','c','v','b','n','m',',','.','/',
        0 /*rshift*/, '*', 0 /*alt*/, ' ', 0 /*capslock*/,
    };
    const char scancode_ascii_shift[128] = {
        0, 27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
        '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
        0, 'A','S','D','F','G','H','J','K','L',':','"','~',
        0, '|','Z','X','C','V','B','N','M','<','>','?',
        0, '*', 0, ' ', 0,
    };

    void push_char(char c) {
        size_t next = (head + 1) % BUF_SIZE;
        if (next != tail) { // drop if full
            buffer[head] = c;
            head = next;
        }
    }

    void keyboard_callback(registers*) {
        uint8_t scancode = inb(0x60);

        if (scancode & 0x80) {
            // key release
            uint8_t released = scancode & 0x7F;
            if (released == 0x2A || released == 0x36) shift_pressed = false;
            return;
        }

        if (scancode == 0x2A || scancode == 0x36) { shift_pressed = true; return; }
        if (scancode >= 128) return;

        char c = shift_pressed ? scancode_ascii_shift[scancode] : scancode_ascii[scancode];
        if (c) push_char(c);
    }
}

void keyboard_install() {
    register_interrupt_handler(33, keyboard_callback); // IRQ1 -> int 33
}

bool keyboard_poll(char* c) {
    if (head == tail) return false;
    *c = buffer[tail];
    tail = (tail + 1) % BUF_SIZE;
    return true;
}

char keyboard_getchar() {
    char c;
    while (!keyboard_poll(&c)) {
        asm volatile ("hlt");
    }
    return c;
}
