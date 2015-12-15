////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

typedef struct page_frame {
    void* link;
    void* virtual;
    size_t size;
    int16_t status;
} page_frame_t;

uint32_t page_directory[1024] __attribute__((aligned(4096)));

uint32_t first_page_table[1024] __attribute__((aligned(4096)));


void clear_page_directory()
{
    int i;
    for(i = 0; i < 1024; i++)
    {
        // This sets the following flags to the pages:
        //   Supervisor: Only kernel-mode can access them
        //   Write Enabled: It can be both read from and written to
        //   Not Present: The page table is not present
        page_directory[i] = 0x00000002;
    }
}

extern void loadPageDirectory(unsigned int*);
extern void enablePaging();


void __attribute__((constructor)) init_paging(void)
{
    clear_page_directory();
    // attributes: supervisor level, read/write, present
    page_directory[0] = ((unsigned int)first_page_table) | 3;

    loadPageDirectory(page_directory);
    enablePaging();
}



