[BITS 32]
MBALIGN     equ  1 << 0                 ; align loaded modules on page boundaries
MEMINFO     equ  1 << 1                 ; provide memory map
VIDINFO     equ  1 << 2
FLAGS       equ  MBALIGN | MEMINFO | VIDINFO      ; this is the Multiboot 'flag' field
MAGIC       equ  0x1BADB002             ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum of above, to prove we are multiboot

section .multiboot
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM
dd 0, 0, 0, 0, 0
dd 0             ; 0 = set graphics mode
dd 80, 25, 16  ; Width, height, depth

section .text
global _start
global halt
extern __init
extern __fini
extern main
extern STACK_TOP
_start:
    ; Set up the stack
    mov esp, STACK_TOP
    ; Push the pointer to the Multiboot information structure.
    push   ebx
    ; Push the magic value. 
    push   eax
    call __init
    sti
    call main
    call __fini
    jmp hang
halt:
    cli
    hlt
hang:
    jmp hang



