
.section ".text"
.global _start
.global halt

_start:
    ldr sp, =STACK_TOP
    bl __init
    bl main
    bl __fini
halt:
    b halt
.size _start, . - _start
