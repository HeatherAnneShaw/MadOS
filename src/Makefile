
AS = nasm
AFLAGS = -f elf32

CFLAGS = -m32 -nostartfiles -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I../include -I./x86
OBJ	= kernel boot.o kernel.o memory.o page.o
FONTS = c64-8.o terminus-16.o
DRIVERS = tables.o timer.o idt.o isr.o irq.o keyboard.o video.o port.o
LIBK = madstd.o stdlib.o string.o stdio.o
EXEC = exec.o elf.o
FS = fs.o fat.o mfs.o
EXTRA = 

all:
	# libk
	gcc $(CFLAGS) -c stdlib.c -o stdlib.o
	gcc $(CFLAGS) -c string.c -o string.o
	gcc $(CFLAGS) -c madstd.c -o madstd.o
	gcc $(CFLAGS) -c stdio.c -o stdio.o
	
	# kernel
	$(AS) $(AFLAGS) x86/boot.s -o boot.o
	gcc $(CFLAGS)  -c kernel.c -o kernel.o
	gcc $(CFLAGS) -c memory.c -o memory.o
	gcc $(CFLAGS) -c x86/page.c -o page.o
	# drivers
	nasm -f elf32 x86/tables.s -o tables.o
	gcc $(CFLAGS) -c x86/port.c -o port.o
	gcc $(CFLAGS) -c x86/vga.c -o video.o
	gcc $(CFLAGS) -c x86/keyboard.c -o keyboard.o
	gcc $(CFLAGS) -c x86/timer.c -o timer.o
	
    # descriptor tables
	gcc $(CFLAGS) -c x86/idt.c -o idt.o
	gcc $(CFLAGS) -c x86/isr.c -o isr.o
	gcc $(CFLAGS) -c x86/irq.c -o irq.o

	# executable formats
	gcc $(CFLAGS) -c exec/exec.c -o exec.o
	gcc $(CFLAGS) -c exec/elf.c -o elf.o
	
	$(AS) -f bin exec/elf32.s -o exec/elf32
	cp exec/elf32 ../os/init.elf
	
	#filesystem formats
	gcc $(CFLAGS) -c fs/fs.c -o fs.o
	gcc $(CFLAGS) -c fs/fat.c -o fat.o
	gcc $(CFLAGS) -c fs/mfs.c -o mfs.o

	# fonts
#	gcc $(CFLAGS) -c x86/fonts/latin-8.c -o latin-8.o
#	gcc $(CFLAGS) -c x86/fonts/latin-16.c -o latin-16.o
	gcc $(CFLAGS) -c x86/fonts/c64-8.c -o c64-8.o
#	gcc $(CFLAGS) -c x86/fonts/c64-16.c -o c64-16.o
#	gcc $(CFLAGS) -c x86/fonts/terminus-8.c -o terminus-8.o
	gcc $(CFLAGS) -c x86/fonts/terminus-16.c -o terminus-16.o
#	gcc $(CFLAGS) -c x86/fonts/matrix-8.c -o matrix-8.o
#	gcc $(CFLAGS) -c x86/fonts/matrix-16.c -o matrix-16.o

	# link it on up
	gcc -T x86/linker.ld $(CFLAGS) -z max-page-size=0x1000 -o $(OBJ) $(FS) $(EXEC) $(DRIVERS) $(FONTS) $(EXTRA) $(LIBK)

test:
	cp kernel ../os/kernel
	qemu-system-i386 -m 4GB -kernel ../os/kernel -append "path=/bin:/sbin:/usr/bin:/usr/sbin" -initrd ../os/mados.img
	awk -v sum="`cat kernel | wc -c -|grep -o "^\S*"`" ' BEGIN {hum[1024^3]="Gb"; hum[1024^2]="Mb"; hum[1024]="Kb"; for (x=1024^3; x>=1024; x/=1024) { if (sum>=x) { printf "%.2f %s\n",sum/x,hum[x]; break; } } if (sum<1024) print "1kb"; } '

clean:
	rm *.o
	rm kernel
