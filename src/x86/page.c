////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <machine.h>
#include <memory.h>

uint64_t page_dir_ptr_tab[4] __attribute__((aligned(0x20))); // must be aligned to (at least)0x20, ...
    // ... turning out that you can put more of them into one page, saving memory
// 512 entries
uint64_t page_dir[1024] __attribute__((aligned(0x1000)));  // must be aligned to page boundary
uint64_t page_tab[4096] __attribute__((aligned(0x1000)));




void init_paging(void)
{
    page_dir_ptr_tab[0] = (uint32_t)&page_dir | 1; // set the page directory into the PDPT and mark it present
    page_dir[0] = (uint32_t)&page_tab | 3; //set the page table into the PD and mark it present/writable

    // map pre kernel, kernel and memory pool
    for(uint32_t i = KERNEL_START, address = 0; i < (KERNEL_END - KERNEL_START)+MEM_POOL_SIZE; i++)
    {
        page_tab[i] = address | 3; // map address and mark it present/writable
        address = address + KERNEL_START;
    }

    asm volatile ("movl %cr4, %eax; bts $5, %eax; movl %eax, %cr4"); // set bit5 in CR4 to enable PAE		 
    asm volatile ("movl %%eax, %%cr3" :: "a" (&page_dir_ptr_tab)); // load PDPT into CR3

    asm volatile ("movl %cr0, %eax; orl $0x80000000, %eax; movl %eax, %cr0;");
}


