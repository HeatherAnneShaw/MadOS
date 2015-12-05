
CFLAGS = -m32 -nostartfiles -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Werror -I../include

all:
	gcc $(CFLAGS) -c stdlib.c -o stdlib.o
	gcc $(CFLAGS) -c string.c -o string.o
	gcc $(CFLAGS) -c madstd.c -o madstd.o
	gcc $(CFLAGS) -c stdio.c -o stdio.o

	nasm -f elf32 x86/boot.s -o boot.o
	gcc -m32 $(CFLAGS) -c kernel.c -o kernel.o
	gcc -m32 $(CFLAGS) -c x86/vga.c -o vga.o
	gcc -m32 -T x86/linker.ld $(CFLAGS) -o kernel boot.o vga.o madstd.o stdlib.o string.o stdio.o kernel.o

clean:
	rm *.o
	rm kernel
