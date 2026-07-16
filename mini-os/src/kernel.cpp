#include "kernel.h"
#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "keyboard.h"
#include "heap.h"

void shell_run();

// Symbol provided by the linker script, marks the end of the kernel image.
extern "C" uint8_t kernel_end;

extern "C" void kernel_main(uint32_t magic, uint32_t /*mb_info_addr*/) {
    terminal::initialize();

    terminal::setcolor(VGA_LIGHT_MAGENTA);
    terminal::write("MiniOS booting...\n");
    terminal::setcolor(VGA_LIGHT_GREY);

    if (magic != 0x2BADB002) {
        terminal::setcolor(VGA_RED);
        terminal::write("Warning: not loaded by a Multiboot-compliant bootloader!\n");
        terminal::setcolor(VGA_LIGHT_GREY);
    }

    terminal::write("[ok] GDT\n");
    gdt_install();

    terminal::write("[ok] IDT\n");
    idt_install();
    irq_install();

    terminal::write("[ok] Heap\n");
    heap_install((uint32_t)&kernel_end, 1024 * 1024); // 1 MiB heap after kernel image

    terminal::write("[ok] Timer (100 Hz)\n");
    timer_install(100);

    terminal::write("[ok] Keyboard\n");
    keyboard_install();

    asm volatile ("sti"); // enable interrupts now that IDT/PIC are ready

    shell_run();

    // shell_run() never returns, but just in case:
    while (true) asm volatile ("hlt");
}
