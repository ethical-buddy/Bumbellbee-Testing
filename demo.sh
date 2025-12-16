#!/bin/bash
# demo.sh - Demonstrates MiniOS kernel features

echo "=========================================="
echo "  Bumbellbee OS v0.1 - Feature Demo"
echo "=========================================="
echo ""
echo "Building kernel..."
make clean > /dev/null 2>&1
make > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed!"
    exit 1
fi

echo "✓ Build successful!"
echo ""
echo "Kernel Features:"
echo "  ✓ GDT (Global Descriptor Table)"
echo "  ✓ IDT (Interrupt Descriptor Table)"
echo "  ✓ PIC (Programmable Interrupt Controller)"
echo "  ✓ PS/2 Keyboard Driver"
echo "  ✓ Serial Console Output"
echo "  ✓ Interrupt-driven I/O"
echo ""
echo "Running kernel with simulated keyboard input..."
echo ""
echo "Kernel output:"
echo "------------------------------------------"

# Run QEMU with keyboard input simulation
(echo "hello world"; sleep 2) | timeout 3 qemu-system-i386 -kernel build/kernel.bin -serial stdio -display none 2>&1 || true

echo "------------------------------------------"
echo ""
echo "To run interactively:"
echo "  make run"
echo ""
echo "Then type and see your keystrokes appear!"
echo "Press Ctrl+A then X to exit QEMU."
