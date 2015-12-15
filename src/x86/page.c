////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>

uint64_t page_dir_ptr_tab[4] __attribute__((aligned(0x20))); // must be aligned to (at least)0x20, ...
    // ... turning out that you can put more of them into one page, saving memory
uint64_t page_directory[512] __attribute__((aligned(0x1000)));  // must be aligned to page boundary
uint64_t page_table[512] __attribute__((aligned(0x1000)));


void clear_page_directory()
{
    for(unsigned int i = 0; i < 1024; i++)
    {
        // This sets the following flags to the pages:
        //   Supervisor: Only kernel-mode can access them
        //   Write Enabled: It can be both read from and written to
        //   Not Present: The page table is not present
        page_directory[i] = 0x00000002;
    }
    for(unsigned int i = 0, address = 0; i < 512; i++)
    {
        page_table[i] = address | 3; // map address and mark it present/writable
        address = address + 0x1000;
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

    printf("Paging enabled\npage_dir_ptr_tab: 0x%x, page_directory: 0x%x, page_table: 0x%x\n\n",
        &page_dir_ptr_tab, &page_directory, &page_table);
}



