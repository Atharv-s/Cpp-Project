CXX = g++
AS = nasm

CXXFLAGS = -m32 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector \
           -Wall -Wextra -Iinclude -O2 -std=c++17
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

BOOT_OBJ = build/boot.o
CPP_SRCS = $(wildcard src/*.cpp)
CPP_OBJS = $(patsubst src/%.cpp,build/%.o,$(CPP_SRCS))

KERNEL_BIN = build/minios.bin
ISO = build/minios.iso

.PHONY: all clean run iso

all: $(KERNEL_BIN)

build:
	mkdir -p build

$(BOOT_OBJ): boot/boot.s | build
	$(AS) $(ASFLAGS) $< -o $@

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(KERNEL_BIN): $(BOOT_OBJ) $(CPP_OBJS) linker.ld
	ld $(LDFLAGS) -o $@ $(BOOT_OBJ) $(CPP_OBJS)

iso: $(KERNEL_BIN)
	mkdir -p iso/boot
	cp $(KERNEL_BIN) iso/boot/minios.bin
	grub-mkrescue -o $(ISO) iso 2>/dev/null

run: iso
	qemu-system-i386 -cdrom $(ISO) -serial stdio

run-nographic: iso
	qemu-system-i386 -cdrom $(ISO) -display none -serial mon:stdio

clean:
	rm -rf build iso/boot/minios.bin
