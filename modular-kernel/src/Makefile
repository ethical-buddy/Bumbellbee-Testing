toolchain=i686-elf-
prefix=../builds/

objs = 	$(prefix)kernel_asm.o \
	$(prefix)kernel.o \
	$(prefix)idt.o \
	$(prefix)irq.o \
	$(prefix)irq.asm.o \
	$(prefix)isr.o \
	$(prefix)isr.asm.o \
	$(prefix)timer.o \
	$(prefix)kb.o \
	$(prefix)screen.o \
	$(prefix)paging.o \
	$(prefix)heap.o \
	$(prefix)exceptions.o \
	$(prefix)keyboard_map.o \
	$(prefix)memory.o \
	$(prefix)mm.o \
	$(prefix)proc.o \

cflags =  -m32 -ffreestanding -g


all:
	nasm -f elf32 -g ../asm/kernel.asm -o $(prefix)kernel_asm.o
	$(toolchain)gcc $(cflags) -c kernel.c -o $(prefix)kernel.o
	$(toolchain)gcc $(cflags) -c idt.c -o $(prefix)idt.o
	$(toolchain)gcc $(cflags) -c irq.c -o $(prefix)irq.o
	$(toolchain)as  ../asm/irq.S -o $(prefix)irq.asm.o
	$(toolchain)gcc $(cflags) -c isr.c -o $(prefix)isr.o
	$(toolchain)as  ../asm/isr.S -o $(prefix)isr.asm.o
	$(toolchain)gcc $(cflags) -c timer.c -o $(prefix)timer.o	
	$(toolchain)gcc $(cflags) -c kb.c -o $(prefix)kb.o
	$(toolchain)gcc $(cflags) -c screen.c  -o $(prefix)screen.o
	$(toolchain)gcc $(cflags) -c paging.c -o $(prefix)paging.o
	$(toolchain)gcc $(cflags) -c heap.c -o $(prefix)heap.o
	$(toolchain)gcc $(cflags) -c exceptions.c -o $(prefix)exceptions.o
	$(toolchain)gcc $(cflags) -c keyboard_map.c -o $(prefix)keyboard_map.o
	$(toolchain)gcc $(cflags) -c memory.c -o $(prefix)memory.o
	$(toolchain)gcc $(cflags) -c mm.c -o $(prefix)mm.o
	$(toolchain)gcc $(cflags) -c proc.c -o $(prefix)proc.o

	$(toolchain)ld -m elf_i386 -T $(prefix)linker.ld -o kernel.bin $(objs)

clean:
	rm $(prefix)*.o


run:
	qemu-system-i386 -kernel kernel.bin

