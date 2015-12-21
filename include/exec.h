////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////


#ifndef __EXEC_H__
#define __EXEC_H__

#include <stdlib.h>
#include <machine.h>

#define MAX_EXEC_FORMATS 10
typedef struct exec_entry {
    char* name;
    bool (*is_type) (void*);
    bool (*load_module) (char*, void*);
} exec_entry_t;

#define DEFAULT_STACK_SIZE (1024 * 6)
#define MAX_PS_NUMBER 250
typedef struct ps_context {
    char* name;     // name of the process
    void* code;     // pointer to actual code
    uint32_t size;  // size of the code
    uint32_t vaddr; // virtual address mapping request
    struct regs context; // process state
    uint32_t stack; // process stack
} ps_context_t;

typedef struct exec_mem_image {
    ps_context_t ps_context;           // process context :P
    uint8_t code[]; // this guy only exists if the ps was loaded from a filesystem
} exec_mem_image_t;

extern ps_context_t* ps_schedule_map[MAX_PS_NUMBER];

extern exec_entry_t* exec_table[MAX_EXEC_FORMATS];
extern size_t registered_exec_handlers;

extern bool register_exec(exec_entry_t* entry);
extern void exec_loadmodule(char* name, void* code, uint32_t vaddr, uint32_t entry, uint32_t size);

extern void exec_add_schedule(ps_context_t* p, uint32_t entry);
extern void exec_end_schedule(ps_context_t* p);


#endif
