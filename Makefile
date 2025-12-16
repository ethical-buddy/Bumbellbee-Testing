# Makefile for OS Kernel

# Compiler and assembler
AS := nasm
CC := gcc
LD := ld

# Flags
ASFLAGS := -f elf32
CFLAGS := -m32 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-stack-protector -nostdlib -nostdinc
LDFLAGS := -m elf_i386 -T linker.ld

# Directories
BOOT_DIR := boot
SRC_DIR := src
BUILD_DIR := build

# Source files
BOOT_ASM := $(BOOT_DIR)/boot.asm
GDT_ASM := $(BOOT_DIR)/gdt.asm
IDT_ASM := $(BOOT_DIR)/idt.asm
KERNEL_C := $(SRC_DIR)/kernel.c

# Object files
BOOT_OBJ := $(BUILD_DIR)/boot.o
GDT_OBJ := $(BUILD_DIR)/gdt.o
IDT_OBJ := $(BUILD_DIR)/idt.o
KERNEL_OBJ := $(BUILD_DIR)/kernel.o

# Output
KERNEL_BIN := $(BUILD_DIR)/kernel.bin
ISO_DIR := $(BUILD_DIR)/iso
ISO_FILE := kernel.iso

# Default target
all: $(KERNEL_BIN)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Assemble boot.asm
$(BOOT_OBJ): $(BOOT_ASM) | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Assemble gdt.asm
$(GDT_OBJ): $(GDT_ASM) | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Assemble idt.asm
$(IDT_OBJ): $(IDT_ASM) | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Compile kernel.c
$(KERNEL_OBJ): $(KERNEL_C) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link everything into kernel binary
$(KERNEL_BIN): $(BOOT_OBJ) $(GDT_OBJ) $(IDT_OBJ) $(KERNEL_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

# Create bootable ISO image
iso: $(KERNEL_BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin
	echo 'set timeout=0' > $(ISO_DIR)/boot/grub/grub.cfg
	echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo 'menuentry "OS Kernel" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '    multiboot2 /boot/kernel.bin' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '    boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_FILE) $(ISO_DIR)

# Run kernel in QEMU with VGA display (uses ISO boot for VGA support)
run: iso
	@echo "Running kernel in QEMU with VGA display..."
	@echo "Close the window or press Ctrl+C to exit"
	qemu-system-i386 -cdrom $(ISO_FILE)

# Run ISO in QEMU
run-iso: iso
	qemu-system-i386 -cdrom $(ISO_FILE)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(ISO_FILE)

.PHONY: all iso run run-iso clean
