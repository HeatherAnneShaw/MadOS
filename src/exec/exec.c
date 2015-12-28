////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <exec.h>
#include <memory.h>
#include <string.h>

exec_entry_t* exec_table[MAX_EXEC_FORMATS] = { NULL };
size_t registered_exec_handlers = 0;
extern ps_context_t main_context;

bool register_exec(exec_entry_t* entry)
{
    if(registered_exec_handlers >= MAX_EXEC_FORMATS) return false;
    exec_table[registered_exec_handlers++] = entry;
    return true;
}

ps_context_t* ps_schedule_map[MAX_PS_NUMBER];

extern void halt(void);
void exec_add_schedule(ps_context_t* p, uint32_t entry)
{
    /*
        struct regs
        {  
            unsigned int gs, fs, es, ds;
            unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
            unsigned int int_no, err_code;
            unsigned int eip, cs, eflags, useresp, ss;
        };
    */
    // setup paging, then context switching / registering
    p->context.eip = entry;
    //p->stack = (uint32_t) malloc(DEFAULT_STACK_SIZE);
    // set the stack to something reasonable using DEFAULT_STACK_SIZE
    //p->context.esp = p->stack + DEFAULT_STACK_SIZE;
    //p->context.ebp = p->stack;

    p->context.esp = entry; // WHY?
    p->context.ebp = entry; // WHY?
    for(int i = 1;i <= MAX_PS_NUMBER;i++)
    {
        if(ps_schedule_map[i] == 0)
        {
            ps_schedule_map[i] = p;
            break;
        }
    }
}

void exec_end_schedule(ps_context_t* p)
{
    for(int i = 0;i <= MAX_PS_NUMBER;i++)
    {
        if(ps_schedule_map[i] == p)
        {
            ps_schedule_map[i] = 0;
            free((void*) p->stack);
            free(p);
            break;
        }
    }
}

void exec_loadmodule(char* name, void* code, uint32_t vaddr, uint32_t entry, uint32_t size)
{
    // load code into context driver and schedule the entry
    ps_context_t* p = malloc(sizeof(ps_context_t));
    mem_entry_t* m = (mem_entry_t*) ((uint32_t) p - sizeof(mem_entry_t));
    m->type = EXE;
    p->name = name;
    p->code = code;
    p->vaddr = vaddr;
    p->size = size;

    extern uint64_t* page_table;
    map_page(page_table, code, vaddr, 3);
    
    exec_add_schedule(p, entry);
    printf("%s ->  padd: 0x%x, vadd: 0x%x, entry: 0x%x, size: %iB\n", name, code, vaddr, entry, size);
}

extern void hang();
static void idle_thread()
{
    puts("Idle thread started, multitasking enabled.");
    hang();
}


void __attribute__((constructor)) init_exec(void)
{
    memset(ps_schedule_map, 0, MAX_PS_NUMBER);
    main_context.name = "kernel";
    main_context.code = (void*) KERNEL_START;
    main_context.vaddr = KERNEL_END;
    main_context.size = 0;
    
    exec_add_schedule(&main_context, (uint32_t) idle_thread);
}



