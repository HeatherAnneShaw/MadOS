////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////


#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdlib.h>
extern uint32_t KERNEL_END;
extern uint32_t KERNEL_START;
extern void halt(void);

extern uint32_t MEM_POOL;
extern uint32_t MEM_POOL_END;
extern uint32_t MEM_POOL_SIZE;

typedef struct mem_entry{
    bool free;
    enum uint8_t {
        PTR,
        STR,
        EXE,
        FS
    } type;
    uint32_t prev;
    uint32_t ptr;
    uint32_t next;
} mem_entry_t;

extern void print_memory_blocks(void);

#endif
