////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <multiboot.h>
#include <assert.h>

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

uint64_t page_dir_ptr_tab[4] __attribute__((aligned(0x20))); // must be aligned to (at least)0x20, ...
    // ... turning out that you can put more of them into one page, saving memory
uint64_t page_directory[512] __attribute__((aligned(0x1000)));  // must be aligned to page boundary
uint64_t page_table[512] __attribute__((aligned(0x1000)));

extern uint32_t KERNEL_END;
extern void halt(void);


void clear_page_directory()
{
    for(unsigned int i = 0, address = 0; i < 512; i++)
    {
        page_table[i] = address | 3; // map address and mark it present/writable
        address = address + 0x1000;
        // This sets the following flags to the pages:
        //   Supervisor: Only kernel-mode can access them
        //   Write Enabled: It can be both read from and written to
        //   Not Present: The page table is not present
        page_directory[i] = 0x00000002;
    }
}

void init_paging(void)
{
    clear_page_directory();
    page_dir_ptr_tab[0] = (uint32_t)&page_directory | 1; // set the page directory into the PDPT and mark it present
    page_directory[0] = (uint32_t)&page_table | 3; //set the page table into the PD and mark it present/writable

    asm volatile ("movl %cr4, %eax; bts $5, %eax; movl %eax, %cr4"); // set bit5 in CR4 to enable PAE		 
    asm volatile ("movl %%eax, %%cr3" :: "a" (&page_dir_ptr_tab)); // load PDPT into CR3
    asm volatile ("movl %cr0, %eax; orl $0x80000000, %eax; movl %eax, %cr0;");

    printf("Paging enabled\npage directory table: 0x%x, page directory: 0x%x, page table: 0x%x\n\n",
        (unsigned) page_dir_ptr_tab, (unsigned) page_directory, (unsigned) page_table);
}


/******************************************************************
    This malloc / free implementation is just temporary so that
    I may have stone age memory management while I work on other
    things.
*******************************************************************/
uint32_t MEM_EARLY_SIZE = 0;
uint32_t MEM_KLUDGE = 0;
uint32_t MEM_KLUDGE_END = 0;
uint32_t MEM_PTR = 0;

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

    if(CHECK_FLAG (mbi->flags, 6))
    {
        for(mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
            (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
            mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
            + mmap->size + sizeof(mmap->size)))
            {
                if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE && mmap->addr >= (uint32_t) &KERNEL_END)
                {
                    MEM_KLUDGE = (uint32_t) &KERNEL_END;
                    MEM_EARLY_SIZE = (uint32_t) mmap->len - (mmap->addr - (uint32_t) &KERNEL_END);
                    break;
                }
            }
    }

skip_multiboot:
    KERNEL_END = (uint32_t) &KERNEL_END;
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

//    init_paging();
}

uint32_t mget_free_block(uint32_t p, size_t size)
{
    while(((mem_entry_t*)p)->ptr != MEM_KLUDGE_END)
    {
        if(((mem_entry_t*)p)->free && (((mem_entry_t*)p)->next - ((mem_entry_t*)p)->ptr) >= size + (sizeof(mem_entry_t) * 2)) break;
        p = ((mem_entry_t*)p)->next;
    }
    return p;
}

void split_block(uint32_t p, size_t size)
{
    mem_entry_t* selected = (mem_entry_t*) p;
    mem_entry_t* split = (mem_entry_t*) (p + size + sizeof(mem_entry_t));
    mem_entry_t* end_marker = (mem_entry_t*) ((mem_entry_t*) p)->next;

    split->free = true;
    split->prev = (uint32_t) selected;
    split->ptr = (uint32_t) split + sizeof(mem_entry_t);
    split->next = (uint32_t) end_marker;

    selected->next = (uint32_t) split;
    end_marker->prev = (uint32_t) split;
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

    // split free block if larger than 
    split_block(p, size);

    // choose one of the two if split and mark not free
    ((mem_entry_t*) p)->free = false;

    return (void*) ((mem_entry_t*) p)->ptr;
}

malloc_t* malloc = malloc_early;

void free(void* ptr)
{
    mem_entry_t* e = ptr - sizeof(mem_entry_t);
    if(e->ptr == (uint32_t) ptr)
        e->free = true;
    // combine contigeous free blocks
    /*for(uint32_t p = MEM_KLUDGE, chunk = 0;((mem_entry_t*)p)->next != MEM_KLUDGE_END;p = ((mem_entry_t*)p)->next)
    {
        if(((mem_entry_t*)p)->free)
        {
            chunk = p;
            
        }
    }*/
}



