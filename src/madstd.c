////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <video.h>

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

ssize_t write(int fildes, const void* buf, size_t nbyte)
{
    switch(fildes)
    {
        default:
            video_writestring(buf, nbyte);
            break;
    }
    return nbyte;
}

ssize_t read(int fildes, void* buf, size_t nbyte)
{
    switch(fildes)
    {
        case (int) stdin:
        buf=buf;
        break;
    }
    return nbyte;
}