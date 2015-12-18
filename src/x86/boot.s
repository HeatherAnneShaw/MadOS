;////////////////////////////////////////////////////////////////////////////////
;// THE SCOTCH-WARE LICENSE (Revision 0):
;// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
;// can do whatever you want with this stuff. If we meet some day, and you think
;// this stuff is worth it, you can buy me a shot of scotch in return
;////////////////////////////////////////////////////////////////////////////////

[BITS 32]
MBALIGN     equ  1 << 0                 ; align loaded modules on page boundaries
MEMINFO     equ  1 << 1                 ; provide memory map
FLAGS       equ  MBALIGN | MEMINFO      ; this is the Multiboot 'flag' field
MAGIC       equ  0x1BADB002             ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum of above, to prove we are multiboot

section .multiboot
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM

section .text
global _start
global halt
global hang
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



