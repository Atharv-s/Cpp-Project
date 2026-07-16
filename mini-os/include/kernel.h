#ifndef KERNEL_H
#define KERNEL_H

using uint8_t  = unsigned char;
using uint16_t = unsigned short;
using uint32_t = unsigned int;
using int32_t  = int;
using size_t   = unsigned int;

// ---- Port I/O -------------------------------------------------------
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
static inline void io_wait() {
    outb(0x80, 0);
}

#endif
