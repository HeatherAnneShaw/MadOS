
#include <stdlib.h>

unsigned char inb(unsigned int port)
{
   unsigned char ret;
   asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
   return ret;
}

void outb(unsigned int port,unsigned char value)
{
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}
