# Quick Start Guide - MiniOS Kernel

## Test the Kernel in 30 Seconds!

### 1. Build
```bash
make
```

### 2. Run
```bash
make run
```

You should see:
```
======================================
  Welcome to Bumbellbee Kernel v0.1
======================================

[*] Initializing GDT...
[OK] GDT loaded

[*] Initializing PIC...
[OK] PIC configured

[*] Initializing IDT...
[OK] IDT loaded

[*] Enabling interrupts...
[OK] Interrupts enabled

NOTE: VGA not available in PVH boot mode.
      Use serial console for output.

System ready! Keyboard typing will appear here...
Type MF!
> _
```

### 3. Type!
**Start typing on your keyboard** - your keystrokes will appear in the terminal!

Try:
- Regular letters: `hello world`
- With shift: `Hello World!`
- Numbers: `12345`
- Symbols: `!@#$%`
- Enter key for newlines

### 4. Exit
Press **Ctrl+A** then **X** to quit QEMU

## What You're Seeing

When you type, here's what happens:

1. **Hardware**: Your keyboard sends a scancode to the PS/2 controller
2. **PIC**: The Programmable Interrupt Controller generates IRQ1
3. **CPU**: Looks up interrupt 33 in the IDT
4. **ISR**: Assembly stub saves registers and calls C handler
5. **Driver**: Converts scancode to ASCII character
6. **Output**: Character sent to serial port (COM1)
7. **Screen**: You see your keystroke!

All of this happens in **microseconds** per keystroke!

## Troubleshooting

**Q: I don't see any output**  
A: Make sure you're using the `make run` command, not just running QEMU directly

**Q: Keyboard doesn't respond**  
A: This is normal - the window doesn't have focus. The serial console (-serial stdio) captures keyboard input

**Q: How do I see VGA output?**  
A: VGA is not available in PVH boot mode. Use serial console instead, or boot via GRUB

**Q: Can I run with a GUI window?**  
A: Yes: `qemu-system-i386 -kernel build/kernel.bin` but VGA won't work - you'll just see the iPXE boot screen

## Next Steps

Want to explore the code?
- `src/kernel.c` - Main kernel with drivers
- `boot/idt.asm` - Interrupt handling
- `boot/gdt.asm` - Memory segmentation

Read `IMPLEMENTATION.md` for detailed technical explanation!

## Features Checklist

- [x] Boots without errors
- [x] Initializes GDT, PIC, IDT
- [x] Enables interrupts
- [x] Responds to keyboard input
- [x] Displays output on serial console
- [x] Works with shift key
- [x] Handles enter and backspace

**Congratulations!** You're running a real operating system kernel that you can interact with!
