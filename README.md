# MiniOS Kernel v0.1

A minimal but **functional** 32-bit x86 operating system kernel with keyboard support, interrupts, and proper system initialization.

## Features

✅ **Global Descriptor Table (GDT)** - Proper memory segmentation  
✅ **Interrupt Descriptor Table (IDT)** - Interrupt handling framework  
✅ **Programmable Interrupt Controller (PIC)** - Hardware interrupt management  
✅ **PS/2 Keyboard Driver** - Full keyboard input with shift key support  
✅ **Serial Console Output** - COM1 serial port communication  
✅ **Interactive Input** - Type and see your input echoed back in real-time!  
✅ **Multiboot2 & PVH Boot** - Compatible with QEMU 10.x

## Project Structure

```
kernel/
├── boot/
│   ├── boot.asm       # Entry point, stack setup
│   ├── gdt.asm        # Global Descriptor Table
│   └── idt.asm        # Interrupt Descriptor Table & ISR stubs
├── src/
│   └── kernel.c       # Main kernel with drivers
├── include/           # Header files (reserved)
├── build/             # Build artifacts (generated)
├── linker.ld          # Memory layout
├── Makefile           # Build system
└── README.md          # This file
```

## Prerequisites

To build and run this kernel, you need:

- **NASM** - The Netwide Assembler
- **GCC** - GNU Compiler Collection with 32-bit support
- **LD** - GNU linker
- **QEMU** - For testing (qemu-system-i386)
- **GRUB** - For creating bootable ISO (optional)
  - `grub-mkrescue` command
  - `xorriso` package

### Installing Dependencies (Arch Linux)

```bash
sudo pacman -S nasm gcc qemu-system-x86 grub xorriso
```

### Installing Dependencies (Ubuntu/Debian)

```bash
sudo apt-get install nasm gcc qemu-system-x86 grub-common xorriso
```

## Building

To build the kernel binary:

```bash
make
```

This will create `build/kernel.bin`.

## Running

### Quick Start - Run with Keyboard Support!

```bash
make run
```

This launches the kernel with an interactive serial console. **Start typing to see your input!**

**Controls:**
- Type any keys - they'll appear on screen
- Shift key works for uppercase and symbols
- Enter for newlines
- **To exit QEMU**: Press `Ctrl+A` then `X`

### Alternative: Run with display window

```bash
qemu-system-i386 -kernel build/kernel.bin
```

Note: VGA output is not available in PVH boot mode (QEMU 10.x limitation), but the kernel boots successfully.

### Create bootable ISO (requires GRUB)

```bash
make iso
make run-iso
```

## Cleaning

To remove all build artifacts:

```bash
make clean
```

## What's Working

- [x] Multiboot2 & PVH boot protocol
- [x] **GDT (Global Descriptor Table)** - Proper segmentation
- [x] **IDT (Interrupt Descriptor Table)** - Full interrupt handling
- [x] **PIC (Programmable Interrupt Controller)** - IRQ remapping
- [x] **PS/2 Keyboard Driver** - Real keyboard input!
- [x] **Serial Console** - COM1 communication
- [x] **Interrupt-driven I/O** - Keyboard interrupts working

## Future Enhancements

- [ ] VGA text mode (requires non-PVH boot)
- [ ] Memory management & paging
- [ ] Process scheduling & multitasking
- [ ] File system support
- [ ] Shell with commands

## Technical Details

### Memory Layout
- **Kernel Base**: 0x100000 (1 MB physical)
- **Stack**: 16 KB
- **Code Segment**: RX permissions (read + execute)
- **Data Segment**: RW permissions (read + write)

### Interrupt Handling Flow
```
Keyboard Press → IRQ1 → INT 33 → ISR Stub → C Handler → Serial Output → EOI
```

### PIC Configuration
- Master PIC: IRQ0-7 remapped to INT 32-39
- Slave PIC: IRQ8-15 remapped to INT 40-47
- Keyboard on IRQ1 (INT 33)

### Boot Process
1. **QEMU** loads kernel via PVH protocol
2. **boot.asm** sets up stack, clears VGA
3. **kernel_main()** initializes GDT, PIC, IDT
4. **Enables interrupts** with `sti`
5. **Waits for interrupts** with `hlt` loop
6. **Keyboard input** triggers IRQ1 → echoes to serial

### Why Serial Instead of VGA?
QEMU 10.x with PVH boot (`-kernel` option) doesn't map VGA memory. The kernel uses COM1 serial port for output instead, which works perfectly for demonstration and debugging!

## License

This project is open source and available for educational purposes.
