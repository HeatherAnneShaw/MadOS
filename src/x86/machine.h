#ifndef __MACHINE_H__
#define __MACHINE_H__

#include <stddef.h>
#include <stdint.h>

struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;    
};


extern unsigned char inb(unsigned int port);
extern void outb(unsigned int port,unsigned char value);

#endif
