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

/******************************************************************
    This malloc / free implementation is just temporary so that
    I may have stone age memory management while I work on other
    things.
*******************************************************************/


uint32_t MEM_EARLY_SIZE = 0;
uint32_t MEM_KLUDGE = 0;
uint32_t MEM_KLUDGE_END = 0;
uint32_t MEM_PTR = 0;
extern uint32_t KERNEL_END;

typedef struct mem_entry{
    bool free;
    uint32_t prev;
    uint32_t ptr;
    uint32_t next;
} mem_entry_t;


void mem_initialize(multiboot_uint32_t magic, multiboot_info_t* mbi)
{
    multiboot_memory_map_t* mmap;
    MEM_EARLY_SIZE = mbi->mem_upper;
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) goto skip_multiboot;

    if (CHECK_FLAG (mbi->flags, 6))
    {
        for (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
            (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
            mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
            + mmap->size + sizeof (mmap->size)))
        {
            if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE && mmap->addr >= &KERNEL_END)
            {
                MEM_KLUDGE = (uint32_t) &KERNEL_END;
                MEM_EARLY_SIZE = (uint32_t) mmap->len - (mmap->addr - (uint32_t) &KERNEL_END);
            }
        }
    }

skip_multiboot:
    KERNEL_END = &KERNEL_END;
    if(MEM_KLUDGE == 0)
        MEM_KLUDGE = KERNEL_END;
    MEM_PTR = MEM_KLUDGE;
    MEM_KLUDGE_END = MEM_KLUDGE + MEM_EARLY_SIZE;

    // first marker block
    uint32_t a_block = MEM_PTR;
    ((mem_entry_t*) a_block)->free = false;
    ((mem_entry_t*) a_block)->prev = MEM_PTR;
    ((mem_entry_t*) a_block)->ptr = MEM_PTR + sizeof(mem_entry_t);
    ((mem_entry_t*) a_block)->next = MEM_PTR + sizeof(mem_entry_t);

    // first entry block
    uint32_t b_block = ((mem_entry_t*) a_block)->next;
    ((mem_entry_t*) b_block)->free = true;
    ((mem_entry_t*) b_block)->prev = a_block;
    ((mem_entry_t*) b_block)->ptr = b_block + sizeof(mem_entry_t);
    ((mem_entry_t*) b_block)->next = MEM_KLUDGE_END - sizeof(mem_entry_t);

    // end marker block
    uint32_t c_block = ((mem_entry_t*) b_block)->next;
    ((mem_entry_t*) c_block)->free = false;
    ((mem_entry_t*) c_block)->prev = b_block;
    ((mem_entry_t*) c_block)->ptr = MEM_KLUDGE_END;
    ((mem_entry_t*) c_block)->next = MEM_KLUDGE_END;

}

uint32_t mget_free_block(uint32_t p, size_t size)
{
    while(((mem_entry_t*)p)->ptr != MEM_KLUDGE_END)
    {
        if(((mem_entry_t*)p)->free && (((mem_entry_t*)p)->next - ((mem_entry_t*)p)->ptr) >= size) break;
        p = ((mem_entry_t*)p)->next;
    }
    return p;
}

void* malloc_early(size_t size)
{
    uint32_t p = MEM_KLUDGE;
    p = mget_free_block(p, size);
    if(((mem_entry_t*)p)->next == MEM_KLUDGE_END || p == MEM_KLUDGE_END)
    {
        panic("malloc", 0);
        halt();
    }

    // split free block if larger than size + sizeof(mem_entry_t) * 2
    

    // choose one of the two if split and mark not free
    ((mem_entry_t*) p)->free = false;

    // combine contigeous free blocks
    

    return (void*) p;
}

malloc_t* malloc = malloc_early;

void free(void* ptr)
{
    mem_entry_t* e = ptr - sizeof(mem_entry_t);
    if(e->ptr == (uint32_t) ptr)
    {
        e->free = true;
    }
}



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

    init_paging();
}

#endif

extern void (*__init_array_start []) (multiboot_uint32_t magic, multiboot_info_t* mbi);
extern void (*__init_array_end []) (multiboot_uint32_t magic, multiboot_info_t* mbi);

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
                    video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK));
                    printf("AVAILABLE ");
                }
                else if(mmap->type == MULTIBOOT_MEMORY_RESERVED)
                {
                    video_setcolor(MAKE_COLOR(COLOR_RED, COLOR_BLACK));
                    printf("RESERVED  ");
                }
                else
                {
                    video_setcolor(MAKE_COLOR(COLOR_LIGHT_BROWN, COLOR_BLACK));
                    printf("UNKNOWN   ");
                }
            video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_BLACK));
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
    /*char* p;
    unsigned int counter = 0;
    puts("\nAllocating / freeing 80MB to test memory manager stability:\n");
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
    video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_BLACK));
    puts("\n");
*/
    uint32_t a = malloc(2);
    free(a);
    printf("%x\n", (a & 0xffffffff));
}
