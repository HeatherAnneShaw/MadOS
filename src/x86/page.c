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

uint64_t pdpt[4] __attribute__((aligned(0x20))); // must be aligned to (at least)0x20, ...
    // ... turning out that you can put more of them into one page, saving memory
uint64_t page_directory[1024] __attribute__((aligned(0x1000)));
uint64_t page_table[1024] __attribute__((aligned(0x1000)));

static inline void __native_flush_tlb_single(uint32_t addr)
{
   asm volatile("invlpg [%0]" ::"r" (addr) : "memory");
}

void map_page(uint64_t* pt, uint32_t physaddr, uint32_t virtualaddr, unsigned int flags)
{
    virtualaddr = (virtualaddr >> 12) << 12;
    page_table[physaddr/0x1000] = virtualaddr | 3; // map address and mark it present/writable
    virtualaddr += 0x1000;
}

void id_page(uint64_t* pt, uint64_t address, int size)
{
    for(unsigned int i = 0;i < size;i++)
    {
        page_table[i] = address | 3; // map address and mark it present/writable
        address += 0x1000;
    }
}

void init_paging(void)
{
    // set the page directory into the PDPT and mark it present
    pdpt[0] = (uint64_t) &page_directory | 1;

    //set the page table into the PD and mark it present/writable
    page_directory[0] = (uint64_t) &page_table | 3;
    page_directory[1] = (uint64_t) &page_table | 3;
    page_directory[2] = (uint64_t) &page_table | 3;
    page_directory[3] = (uint64_t) &page_table | 3;

    // identity map the first 4MB
    id_page(page_table, 0, 1024);

     // set bit5 in CR4 to enable PAE
    asm volatile (
      "mov eax, cr4\n"
      "bts eax, 5\n"
      "mov cr4, eax\n"
    );
    
    // load PDPT into CR3
    asm volatile ("mov cr3, eax" :: "a" (&pdpt));
    
    // enable paging
    asm volatile (
        "mov eax, cr0\n"
        "or eax, 0x80000000\n"
        "mov cr0, eax\n"
    );
}


