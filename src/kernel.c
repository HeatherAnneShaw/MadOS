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

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

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
    
    printf("flags = 0b%b\n", (unsigned) mbi->flags);
     
    // Is boot_device valid? 
    if(CHECK_FLAG(mbi->flags, 1))
        printf("boot_device = 0x%x\n", (unsigned) mbi->boot_device);    
        
    // Is the command line passed? 
    if(CHECK_FLAG(mbi->flags, 2))
        printf("cmdline = %s\n", (char*) mbi->cmdline);
     
    // Are mods_* valid? 
    if(CHECK_FLAG(mbi->flags, 3))
    {
        multiboot_module_t *mod;
           
        int i;
        printf("mods_count = %i, mods_addr = 0x%x\n",
            (int) mbi->mods_count, (int) mbi->mods_addr);
        for(i = 0, mod = (multiboot_module_t *) mbi->mods_addr;
                i < (int) mbi->mods_count;
                i++, mod++)
            printf(" mod_start = 0x%x, mod_end = 0x%x, cmdline = %s\n",
                (unsigned) mod->mod_start,
                (unsigned) mod->mod_end,
                (char *) mod->cmdline);
    }
     
    // Bits 4 and 5 are mutually exclusive! 
    if(CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
    {
        printf("Both bits 4 and 5 are set. This cannot be!!!\n");
        return;
    }
    
    // Is the symbol table of a.out valid? 
    if(CHECK_FLAG (mbi->flags, 4))
    {
        multiboot_aout_symbol_table_t *multiboot_aout_sym = &(mbi->u.aout_sym);
        printf("multiboot_aout_symbol_table: tabsize = 0x%0x, "
            "strsize = 0x%x, addr = 0x%x\n",
            (unsigned) multiboot_aout_sym->tabsize,
            (unsigned) multiboot_aout_sym->strsize,
            (unsigned) multiboot_aout_sym->addr);
    }
     
    // Is the section header table of ELF valid? 
    if(CHECK_FLAG(mbi->flags, 5))
    {
        multiboot_elf_section_header_table_t *multiboot_elf_sec = &(mbi->u.elf_sec);
        printf("multiboot_elf_sec: num = %u, size = 0x%x,"
            " addr = 0x%x, shndx = 0x%x\n\n",
            (unsigned) multiboot_elf_sec->num, (unsigned) multiboot_elf_sec->size,
            (unsigned) multiboot_elf_sec->addr, (unsigned) multiboot_elf_sec->shndx);
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

    char* a = malloc(4);
    strcpy("AAA", a);

    char* b = malloc(2);
    strcpy("B", b);

    char* c = malloc(2);
    strcpy("C", c);

    char* d = malloc(2);
    strcpy("D", d);

    extern void print_memory_blocks(void);
    putch('\n');
    print_memory_blocks();puts(" -> a[4] = \"AAA\", b[2] = \"B\", c[2] = \"C\", d[2] = \"D\"");
    free(d);print_memory_blocks();puts(" -> free(d)");
    free(b);print_memory_blocks();puts(" -> free(b)");
    free(c);print_memory_blocks();puts(" -> free(c)");
    free(a);print_memory_blocks();puts(" -> free(a)");

    char* p;
    unsigned int counter = 0;
    puts("\nAllocating / freeing 80MB in 1KB blocks to test memory manager stability:\n");
    while(counter < 1024 * 80)
    {
        p = malloc(1024);
        memset(p, 'X', 1024);
        free(p);
        if(++counter % 1024 == 0)
        {
            video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_LIGHT_GREEN));
            putchar('.');
        }
    }
    video_setcolor(DEFAULT_COLOR);
    puts("\n");
    
    d = malloc(10);
    memset(d, 'D', 9);
    d[9] = 0;
    print_memory_blocks();puts(" -> d[10] = \"DDDDDDDDD\"");
    printf("\na = \"%s\"\n\n", a);
    free(d);print_memory_blocks();puts(" -> free(d)");
}
