
CFLAGS = -m32 -nostartfiles -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Werror -I../include
OBJ    = kernel boot.o video.o port.o madstd.o stdlib.o string.o stdio.o kernel.o

all:
	# libk
	gcc $(CFLAGS) -c stdlib.c -o stdlib.o
	gcc $(CFLAGS) -c string.c -o string.o
	gcc $(CFLAGS) -c madstd.c -o madstd.o
	gcc $(CFLAGS) -c stdio.c -o stdio.o
	
	# kernel
	nasm -f elf32 x86/boot.s -o boot.o
	gcc $(CFLAGS) -c x86/port.c -o port.o
	gcc $(CFLAGS) -c x86/vga.c -o video.o
	gcc $(CFLAGS) -c kernel.c -o kernel.o
	
	# link it on up
	gcc -T x86/linker.ld $(CFLAGS) -o $(OBJ)

clean:
	rm *.o
	rm kernel
