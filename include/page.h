////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////


#ifndef __PAGE_H__
#define __PAGE_H__

#include <stdlib.h>
#include <machine.h>

extern void map_page(uint64_t* pt, uint32_t physaddr, uint32_t virtualaddr, unsigned int flags);
extern void init_paging(void);

#endif
