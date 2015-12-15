////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <multiboot.h>

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
    page_dir_ptr_tab[0] = (uint64_t)&page_directory | 1; // set the page directory into the PDPT and mark it present
    page_directory[0] = (uint64_t)&page_table | 3; //set the page table into the PD and mark it present/writable

    asm volatile ("movl %cr4, %eax; bts $5, %eax; movl %eax, %cr4"); // set bit5 in CR4 to enable PAE		 
    asm volatile ("movl %%eax, %%cr3" :: "a" (&page_dir_ptr_tab)); // load PDPT into CR3
    asm volatile ("movl %cr0, %eax; orl $0x80000000, %eax; movl %eax, %cr0;");

    printf("Paging enabled\npage directory table: 0x%x, page directory: 0x%x, page table: 0x%x\n\n",
        &page_dir_ptr_tab, &page_directory, &page_table);
}


/******************************************************************
    This malloc / free implementation is just temporary so that
    I may have stone age memory management while I work on other
    things.
*******************************************************************/


#define MEM_EARLY_SIZE 1024 * 3   // this guy needs to become an integer set to free - kernel_end
char* MEM_KLUDGE;
char* MEM_KLUDGE_END;
char* MEM_PTR;

mem_entry_t MEM_FLAT_TABLE[MEM_EARLY_SIZE];
mem_entry_t* MEM_FLAT_TABLE_TOP = MEM_FLAT_TABLE;
void mem_initialize(multiboot_uint32_t magic, multiboot_info_t* mbi)
{
    multiboot_memory_map_t* mmap;
    while(mmap < mbi->mmap_addr + mbi->mmap_length)
    {
        mmap = (multiboot_memory_map_t*) ((unsigned int)mmap + mmap->size + sizeof(unsigned int));
    }
    
    MEM_KLUDGE = KERNEL_END;
    MEM_PTR = MEM_KLUDGE;
    MEM_KLUDGE_END = MEM_KLUDGE + MEM_EARLY_SIZE;

    for(unsigned i = 0;i < MEM_EARLY_SIZE;i++)
    {
        MEM_KLUDGE[i] = 0;
        (MEM_FLAT_TABLE+i)->ptr = NULL;
        (MEM_FLAT_TABLE+i)->size = 0;
    }
}

void* malloc_early(size_t size)
{
    MEM_PTR += size;
    if(MEM_PTR >= MEM_KLUDGE_END)
    {
        panic("malloc", 0);
        halt();
    }
    
    MEM_FLAT_TABLE_TOP->ptr = MEM_PTR - size;
    MEM_FLAT_TABLE_TOP->size = size;
    MEM_FLAT_TABLE_TOP += 1;
    return MEM_PTR - size;
}

malloc_t* malloc = malloc_early;

void free(void* ptr)
{
    int i;
    for(i = 0;i <= MEM_EARLY_SIZE;i++)
    {
        if((MEM_FLAT_TABLE+i)->ptr == ptr)
        {
            (MEM_FLAT_TABLE+i)->ptr = NULL;
            if(ptr + (MEM_FLAT_TABLE+i)->size == MEM_PTR)
            {
                MEM_PTR -= (MEM_FLAT_TABLE+i)->size;
                (MEM_FLAT_TABLE+i)->size = 0;
            }
            break;
        }
    }
    if(i == MEM_EARLY_SIZE) return;
    if((MEM_FLAT_TABLE + i) == (MEM_FLAT_TABLE_TOP - 1))
    {
        MEM_FLAT_TABLE_TOP -= 1;
    }
}




