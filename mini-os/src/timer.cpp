#include "timer.h"
#include "idt.h"

static volatile uint32_t ticks = 0;

static void timer_callback(registers*) {
    ticks++;
}

void timer_install(uint32_t frequency_hz) {
    register_interrupt_handler(32, timer_callback); // IRQ0 -> int 32

    uint32_t divisor = 1193180 / frequency_hz;
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

uint32_t timer_ticks() {
    return ticks;
}

void sleep_ticks(uint32_t n) {
    uint32_t target = ticks + n;
    while (ticks < target) {
        asm volatile ("hlt");
    }
}
