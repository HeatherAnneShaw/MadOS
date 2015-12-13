#////////////////////////////////////////////////////////////////////////////////
#// THE SCOTCH-WARE LICENSE (Revision 0):
#// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
#// can do whatever you want with this stuff. If we meet some day, and you think
#// this stuff is worth it, you can buy me a shot of scotch in return
#///////////////////////////////////////////////////////////////////////////////

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
