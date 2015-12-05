#ifndef __MACHINE_H__
#define __MACHINE_H__

#include <stddef.h>
#include <stdint.h>

extern unsigned char inb(unsigned int port);
extern void outb(unsigned int port,unsigned char value);

#endif
