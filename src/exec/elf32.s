[bits 32]
section .text
global _start

;org 0x1337
%define KLUDGE 0x11d054
org 0x11d054
ehdr:                                  ; Elf32_Ehdr
	db 0x7F, "ELF", 1, 1, 1            ;   e_ident
	times 9 db 0
	dw 2                               ;   e_type
	dw 3                               ;   e_machine
	dd 1                               ;   e_version
	dd _start                          ;   e_entry
	dd phdr - $$                       ;   e_phoff
	dd 0                               ;   e_shoff
	dd 0                               ;   e_flags
	dw ehdrsize                        ;   e_ehsize
	dw phdrsize                        ;   e_phentsize
	dw 1                               ;   e_phnum
	dw 0                               ;   e_shentsize
	dw 0                               ;   e_shnum
	dw 0                               ;   e_shstrndx
	ehdrsize equ $ - ehdr
	
phdr:                                  ; Elf32_Phdr
	dd 1                               ;   p_type
	dd headersize                      ;   p_offset
	dd $$                              ;   p_vaddr
	dd $$                              ;   p_paddr
	dd filesize                        ;   p_filesz
	dd program_size                    ;   p_memsz
	dd 7                               ;   p_flags
	dd 0                               ;   p_align
	phdrsize equ $ - phdr

headersize equ ehdrsize + phdrsize

p_start:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
string:
    db "Hello world, I'm a kernel module!!!", 0xa

_start:
    mov eax, 4
    mov ebx, 1
    lea ecx, [KLUDGE]
    mov edx, 36
    int 31

back: jmp back  ; no exit syscall yet ...


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
filesize equ $ - $$
program_size equ $ - p_start



