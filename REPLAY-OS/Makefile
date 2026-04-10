CC = gcc
LD = ld
AS = nasm

CFLAGS = -ffreestanding -m64 -fno-stack-protector -fno-pie -fno-pic -mno-red-zone -Iinclude -nostdlib -c
LDFLAGS = -T linker.ld

all: os-image.bin

boot.bin: boot/boot.asm boot/print.asm
	$(AS) -f bin boot/boot.asm -o build/boot.bin

kernel_entry.o: boot/kernel_entry.asm boot/gdt.asm
	$(AS) -f elf64 boot/kernel_entry.asm -o build/kernel_entry.o

kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) kernel/kernel.c -o build/kernel.o

serial.o: kernel/serial.c
	$(CC) $(CFLAGS) kernel/serial.c -o build/serial.o

vga.o: kernel/vga.c
	$(CC) $(CFLAGS) kernel/vga.c -o build/vga.o

idt.o: kernel/idt.c
	$(CC) $(CFLAGS) kernel/idt.c -o build/idt.o

interrupt.o: kernel/interrupt.asm
	$(AS) -f elf64 kernel/interrupt.asm -o build/interrupt.o

timer.o: kernel/timer.c
	$(CC) $(CFLAGS) kernel/timer.c -o build/timer.o

keyboard.o: kernel/keyboard.c
	$(CC) $(CFLAGS) kernel/keyboard.c -o build/keyboard.o

pmm.o: kernel/pmm.c
	$(CC) $(CFLAGS) kernel/pmm.c -o build/pmm.o

task.o: kernel/task.c
	$(CC) $(CFLAGS) kernel/task.c -o build/task.o

syscall.o: kernel/syscall.c
	$(CC) $(CFLAGS) kernel/syscall.c -o build/syscall.o

ide.o: kernel/ide.c
	$(CC) $(CFLAGS) kernel/ide.c -o build/ide.o

shell.o: kernel/shell.c
	$(CC) $(CFLAGS) kernel/shell.c -o build/shell.o

trace.o: kernel/trace.c
	$(CC) $(CFLAGS) kernel/trace.c -o build/trace.o

trace_disk.o: kernel/trace_disk.c
	$(CC) $(CFLAGS) kernel/trace_disk.c -o build/trace_disk.o

replay.o: kernel/replay.c
	$(CC) $(CFLAGS) kernel/replay.c -o build/replay.o

workload.o: kernel/workload.c
	$(CC) $(CFLAGS) kernel/workload.c -o build/workload.o

kernel.bin: kernel_entry.o kernel.o serial.o vga.o idt.o interrupt.o timer.o keyboard.o pmm.o task.o syscall.o ide.o shell.o trace.o trace_disk.o replay.o workload.o
	$(LD) $(LDFLAGS) build/kernel_entry.o build/kernel.o build/serial.o build/vga.o build/idt.o build/interrupt.o build/timer.o build/keyboard.o build/pmm.o build/task.o build/syscall.o build/ide.o build/shell.o build/trace.o build/trace_disk.o build/replay.o build/workload.o -o build/kernel.bin

os-image.bin: boot.bin kernel.bin
	cat build/boot.bin build/kernel.bin > os-image.bin
	truncate -s 1M os-image.bin

run: all
	qemu-system-x86_64 -drive format=raw,file=os-image.bin -serial stdio

debug: all
	qemu-system-x86_64 -drive format=raw,file=os-image.bin -serial stdio -d int,cpu_reset -D qemu.log -no-reboot

clean:
	rm -rf build/*.o build/*.bin os-image.bin qemu.log
