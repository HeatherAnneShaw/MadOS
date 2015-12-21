////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdint.h>
#include <multiboot.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <module.h>
#include <video.h>
#include <exec.h>
#include <memory.h>
#include <fs.h>

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

// kernel registration tables


#if defined(__i386__)

extern void gdt_install();
extern void idt_install();
extern void isrs_install();
extern void irq_install();
extern void init_paging();

void __attribute__((constructor)) handler_initialize(void)
{
    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
}

#endif

extern void (*__init_array_start []) (multiboot_uint32_t magic, multiboot_info_t* mbi);
extern void (*__init_array_end []) (multiboot_uint32_t magic, multiboot_info_t* mbi);
extern void mem_initialize();
void __init(multiboot_uint32_t magic, multiboot_info_t* mbi)
{
    mem_initialize(magic, mbi);   // initialize stage 1 memory manager
    //init_paging();
    
    size_t i = __init_array_end - __init_array_start;
    while(i--)
        (*__init_array_start[i])(magic, mbi);
}


extern void (*__fini_array_start []) (void);
extern void (*__fini_array_end []) (void);
void __fini(void)
{
    size_t i = __fini_array_end - __fini_array_start;
    while(i--)
        (*__fini_array_start[i])();
}

void main(multiboot_uint32_t magic, multiboot_info_t* mbi)
{
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        goto skip_multiboot;
    }

    // Is boot_device valid? 
    if(CHECK_FLAG(mbi->flags, 1))
        printf("\nboot_device = 0x%x\n", (unsigned) mbi->boot_device);    
    // Is the command line passed? 
    if(CHECK_FLAG(mbi->flags, 2))
        printf("\ncmdline = %s\n", (char*) mbi->cmdline);
     
    // Are mods_* valid? 
    if(CHECK_FLAG(mbi->flags, 3))
    {
        multiboot_module_t *mod;
           
        int i;
        printf("\nmods_count = %i, mods_addr = 0x%x\n",
            (int) mbi->mods_count, (int) mbi->mods_addr);
        for(i = 0, mod = (multiboot_module_t *) mbi->mods_addr;
                i < (int) mbi->mods_count;
                i++, mod++)
                {
                    // detect module type
                    unsigned place;
                    for(place = 0;place < registered_exec_handlers;place++)
                    {
                        // ramdisks
                        if(fs_table[i]->is_type((void*) mod->mod_start) == true)
                        {
                            puts("WOOT!!!");
                            fs_table[i]->load_module((char*) mod->cmdline, (void*) mod->mod_start);
                            break;
                        }
                        // executables
                        if(exec_table[i]->is_type((void*) mod->mod_start))
                        {
                            exec_table[i]->load_module((char*) mod->cmdline, (void*) mod->mod_start);
                            break;
                        }
                    }
                    printf("mod_start = 0x%x, mod_end = 0x%x\n",
                        exec_table[place]->name,
                        (unsigned) mod->mod_start,
                        (unsigned) mod->mod_end);
                }
    }
    if(CHECK_FLAG(mbi->flags, 0))
        printf("\nmem_lower = %uKB, mem_upper = %uKB\n",
            (unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);
    // Are mmap_* valid?
    if (CHECK_FLAG (mbi->flags, 6))
    {
        multiboot_memory_map_t* mmap;
        printf ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
            (unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
        for (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
            (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
            mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
            + mmap->size + sizeof (mmap->size)))
        {
            {
                if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
                {
                    video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREEN, DEFAULT_BG_COLOR));
                    printf("AVAILABLE ");
                }
                else if(mmap->type == MULTIBOOT_MEMORY_RESERVED)
                {
                    video_setcolor(MAKE_COLOR(COLOR_RED, DEFAULT_BG_COLOR));
                    printf("RESERVED  ");
                }
                else
                {
                    video_setcolor(MAKE_COLOR(COLOR_LIGHT_BROWN, DEFAULT_BG_COLOR));
                    printf("UNKNOWN   ");
                }
            video_setcolor(DEFAULT_COLOR);
            printf ("-> size = 0x%x, base_addr = 0x%x%x,"
                " length = 0x%x%x\n",
                (unsigned) (mmap->size),
                (unsigned) (mmap->addr >> 32),
                (unsigned) (mmap->addr & 0xffffffff),
                (unsigned) (mmap->len >> 32),
                (unsigned) (mmap->len & 0xffffffff));
            }
        }
    }
skip_multiboot: ({}); // labels must be part of a statement
    print_memory_blocks();
}
