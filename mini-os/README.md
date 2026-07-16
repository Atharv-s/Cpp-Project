# MiniOS

A small, real, bare-metal x86 operating system written in C++. It boots via
GRUB (Multiboot), runs in QEMU or on real hardware, and drops you into an
interactive shell. This was built and boot-tested end-to-end in QEMU — it's
not a simulation.

## Features

- **Boots via GRUB** using the Multiboot specification (`boot/boot.s`)
- **GDT** — flat memory model, kernel + user mode segments (`src/gdt.cpp`)
- **IDT / ISRs / IRQs** — full interrupt handling with PIC remapping (`src/idt.cpp`)
- **PIT timer** driver running at 100 Hz (`src/timer.cpp`)
- **PS/2 keyboard** driver with a scancode table and circular input buffer (`src/keyboard.cpp`)
- **VGA text-mode** driver with scrolling, colors, and cursor control (`src/vga.cpp`)
- **Bump-pointer heap allocator** (`src/heap.cpp`)
- **Freestanding libc**-style string helpers (`src/string.cpp`)
- **Interactive shell** (`src/shell.cpp`) with commands:
  - `help`, `about`, `clear`, `echo <text>`, `uptime`
  - `calc <a> <op> <b>` — integer calculator (`+ - * /`)
  - `colors` — VGA palette demo
  - `reboot`

## Requirements

### Windows

The toolchain here (nasm, GRUB tools, xorriso, QEMU) is Linux-native tooling.
The reliable way to build this on Windows is **WSL2** — it runs a real Linux
kernel, so everything below works unmodified.

1. **Install WSL2** (skip if you already have it). Open PowerShell as
   Administrator and run:
   ```powershell
   wsl --install
   ```
   Reboot if prompted. This installs Ubuntu by default.

2. **Open Ubuntu** (search "Ubuntu" in the Start menu) and install the
   toolchain:
   ```bash
   sudo apt-get update
   sudo apt-get install -y nasm gcc-multilib g++-multilib grub-pc-bin \
                            grub-common xorriso qemu-system-x86 mtools
   ```

3. **Copy the project into WSL and build.** From inside the Ubuntu shell,
   your Windows files are at `/mnt/c/Users/<you>/...`. Easiest is to copy
   the unzipped `mini-os` folder into your Linux home directory first (WSL's
   filesystem is much faster than `/mnt/c`):
   ```bash
   cp -r /mnt/c/Users/<you>/Downloads/mini-os ~/mini-os
   cd ~/mini-os
   make run
   ```

4. **Graphical QEMU window on Windows:** WSL2 on a current version of
   Windows 11 forwards Linux GUI apps to the desktop automatically
   (WSLg) — `make run` should just pop up a QEMU window. If nothing
   appears, run `make iso` instead, then boot the resulting
   `build/minios.iso` with a native Windows build of QEMU
   (https://www.qemu.org/download/#windows) via:
   ```powershell
   qemu-system-i386.exe -cdrom build\minios.iso
   ```

### Linux (native) / macOS

On Ubuntu/Debian:

```bash
sudo apt-get install nasm gcc-multilib g++-multilib grub-pc-bin grub-common \
                      xorriso qemu-system-x86 mtools
```

On macOS, install a cross-compiler (`brew install i686-elf-gcc nasm xorriso qemu`)
and adjust the `Makefile`'s `CXX`/`AS` variables to point at the cross tools,
since macOS's native GCC/Clang can't produce 32-bit ELF binaries directly.

## Build & Run

```bash
make          # builds build/minios.bin (the raw kernel ELF)
make iso      # packages it into a bootable build/minios.iso with GRUB
make run      # builds the ISO and boots it in QEMU with a graphical window
```

If you just want the raw kernel binary without a bootable ISO:

```bash
make
```

To clean build artifacts:

```bash
make clean
```

## How it boots

1. GRUB loads `build/minios.bin`, recognizes the Multiboot header at the
   start of the file, and jumps to `_start` in `boot/boot.s`.
2. `_start` sets up a stack and calls `kernel_main()` in `src/kernel.cpp`.
3. `kernel_main` initializes the VGA terminal, GDT, IDT/PIC, heap, PIT timer,
   and PS/2 keyboard, enables interrupts, then hands off to `shell_run()`.
4. The shell reads keystrokes from the keyboard driver's buffer (filled by
   the IRQ1 handler) and executes built-in commands.

## Project layout

```
boot/boot.s       Multiboot header, entry point, ISR/IRQ assembly stubs, GDT/IDT loaders
src/kernel.cpp    kernel_main() — boot sequence
src/gdt.cpp       Global Descriptor Table setup
src/idt.cpp       Interrupt Descriptor Table, PIC remap, interrupt dispatch
src/timer.cpp     Programmable Interval Timer (IRQ0)
src/keyboard.cpp  PS/2 keyboard driver (IRQ1)
src/vga.cpp       VGA text-mode terminal driver
src/heap.cpp      Bump-pointer allocator
src/string.cpp    Freestanding string/memory functions
src/shell.cpp     Interactive command shell
include/          Headers for all of the above
linker.ld         Places the kernel at the 1MiB mark
iso/boot/grub/    GRUB configuration for the bootable ISO
```

## Extending it

Ideas if you want to keep going:
- A real memory allocator (free-list instead of bump allocator)
- Paging / virtual memory
- A simple filesystem (even a RAM disk with a flat file table)
- User-mode processes and a syscall interface (segments for this are
  already set up in the GDT)
- Multitasking via the PIT interrupt (a task switcher)
- Serial port driver for easier debugging with `qemu -serial stdio`
