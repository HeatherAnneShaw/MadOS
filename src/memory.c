////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <multiboot.h>
#include <video.h>

#include <memory.h>
#include <shell.h>

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

/*****************************************
    Flat memory manager from scratch :D
*****************************************/

uint32_t MEM_POOL = 0;
uint32_t MEM_POOL_END = 0;
uint32_t MEM_POOL_SIZE = 0;

void mem_initialize(multiboot_uint32_t magic, multiboot_info_t* mbi)
{
    multiboot_memory_map_t* mmap;
    MEM_POOL_SIZE = mbi->mem_upper;
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
                    MEM_POOL_SIZE = (uint32_t) mmap->len - (mmap->addr - (uint32_t) &KERNEL_END);
                    break;
                }
            }
    }

skip_multiboot:
    MEM_POOL = (uint32_t) &KERNEL_END;
    MEM_POOL_END = MEM_POOL + MEM_POOL_SIZE;

    // first marker block
    mem_entry_t* a_block =(mem_entry_t*) MEM_POOL;
    a_block->free = false;
    a_block->prev = MEM_POOL;
    a_block->ptr = MEM_POOL + sizeof(mem_entry_t);
    a_block->next = MEM_POOL + sizeof(mem_entry_t);

    // first entry block
    mem_entry_t* b_block = (mem_entry_t*) a_block->next;
    b_block->free = true;
    b_block->prev = (uint32_t) a_block;
    b_block->ptr = (uint32_t) b_block + sizeof(mem_entry_t);
    b_block->next = MEM_POOL_END - sizeof(mem_entry_t);

    // end marker block
    mem_entry_t* c_block = (mem_entry_t*) b_block->next;
    c_block->free = false;
    c_block->prev = (uint32_t) b_block;
    c_block->ptr = MEM_POOL_END;
    c_block->next = MEM_POOL_END;
}


void combine_free_blocks()
{
    // combine contigeous free blocks
    for(uint32_t p = MEM_POOL;((mem_entry_t*)p)->next != MEM_POOL_END;p = ((mem_entry_t*)p)->next)
    {
        if(((mem_entry_t*)p)->free)
        {
            for(mem_entry_t* c = (mem_entry_t*) p;;c = (mem_entry_t*) c->next)
            {
                if(! c->free)
                {
                    if(c->prev != p)
                    {
                        ((mem_entry_t*)p)->next = (uint32_t) c;
                        c->prev = p;
                    }
                    else break;
                }
                if(c->next == MEM_POOL_END) break;
            }
        }
    }
}


uint32_t mget_free_block(uint32_t p, size_t size)
{
    combine_free_blocks();
    while(((mem_entry_t*)p)->ptr != MEM_POOL_END)
    {
        if(((mem_entry_t*)p)->free && (((mem_entry_t*)p)->next - ((mem_entry_t*)p)->ptr) >= size) break;
        p = ((mem_entry_t*)p)->next;
    }
    return p;
}

void split_block(uint32_t p, size_t size)
{
    mem_entry_t* selected = (mem_entry_t*) p;
    mem_entry_t* split = (mem_entry_t*) (p + sizeof(mem_entry_t) + size);
    mem_entry_t* end_marker = (mem_entry_t*) ((mem_entry_t*) p)->next;

    split->free = true;
    split->prev = (uint32_t) selected;
    split->ptr = (uint32_t) split + sizeof(mem_entry_t);
    split->next = (uint32_t) end_marker;

    selected->next = (uint32_t) split;
    end_marker->prev = (uint32_t) split;
}

void* malloc_flat(size_t size)
{
    mem_entry_t* p = (mem_entry_t*) MEM_POOL;
    p = (mem_entry_t*) mget_free_block((uint32_t) p, size);
    if(p->next == (uint32_t) MEM_POOL_END || (uint32_t) p == MEM_POOL_END)
    {
        panic("malloc", 0);
        halt();
    }

    // split free block
    if(p->next - p->ptr >= size + (sizeof(mem_entry_t) * 2))
        split_block((uint32_t) p, size);

    // choose one of the two if split and mark not free
    p->free = false;
    p->type = PTR;

    return (void*) p->ptr;
}

malloc_t* malloc = malloc_flat;

void free(void* ptr)
{
    mem_entry_t* e = ptr - sizeof(mem_entry_t);
    if(e->ptr == (uint32_t) ptr)
        e->free = true;
}


void print_memory_blocks(void)
{
    asm("cli");
    combine_free_blocks();
    for(mem_entry_t* p = (mem_entry_t*)((mem_entry_t*) MEM_POOL)->next;p->next != MEM_POOL_END;p = (mem_entry_t*) p->next)
    {
        video_setcolor(DEFAULT_COLOR);
        putch('|');
        if(p->free)
        {
            video_setcolor(MAKE_COLOR(COLOR_BLACK, COLOR_GREEN));
            putchar('*');
        }
        else
        {
            uint32_t psize = p->next - p->ptr;
            if(p->type == STR)
            {
                video_setcolor(MAKE_COLOR(COLOR_WHITE, COLOR_BLUE));
                if(psize / (1024*1024*1024) > 0)
                    printf("KS %i.%iGB", psize / (1024*1024*1024), psize % (1024*1024*1024));
                else if(psize / (1024*1024) > 0)
                    printf("KS %i.%iMB", psize / (1024*1024), psize % (1024*1024));
                else
                    printf("KS %i.%iKB", psize / 1024, psize % 1024);
            }
            else if(p->type == PTR)
            {
                video_setcolor(MAKE_COLOR(COLOR_BLACK, COLOR_WHITE));
                if(psize / (1024*1024*1024) > 0)
                    printf("PT %i.%iGB", psize / (1024*1024*1024), psize % (1024*1024*1024));
                else if(psize / (1024*1024) > 0)
                    printf("PT %i.%iMB", psize / (1024*1024), psize % (1024*1024));
                else
                    printf("PT %i.%iKB", psize / 1024, psize % 1024);
            }
            else if(p->type == EXE)
            {
                video_setcolor(MAKE_COLOR(COLOR_BLACK, COLOR_LIGHT_CYAN));
                if(psize / (1024*1024*1024) > 0)
                    printf("EX %i.%iGB", psize / (1024*1024*1024), psize % (1024*1024*1024));
                else if(psize / (1024*1024) > 0)
                    printf("EX %i.%iMB", psize / (1024*1024), psize % (1024*1024));
                else
                    printf("EX %i.%iKB", psize / 1024, psize % 1024);
            }
            else if(p->type == FS)
            {
                video_setcolor(MAKE_COLOR(COLOR_WHITE, COLOR_RED));
                if(psize / (1024*1024*1024) > 0)
                    printf("FS %i.%iGB", psize / (1024*1024*1024), psize % (1024*1024*1024));
                else if(psize / (1024*1024) > 0)
                    printf("FS %i.%iMB", psize / (1024*1024), psize % (1024*1024));
                else
                    printf("FS %i.%iKB", psize / 1024, psize % 1024);
            }
            video_setcolor(DEFAULT_COLOR);
        }
    }
    video_setcolor(DEFAULT_COLOR);
    printf("\n\nkey:\n| = %iB mem_entry struct\n", sizeof(mem_entry_t));
    video_setcolor(MAKE_COLOR(COLOR_BLACK, COLOR_GREEN));
    putchar('*');
    video_setcolor(DEFAULT_COLOR);
    puts(" -> free block");

    video_setcolor(MAKE_COLOR(COLOR_WHITE, COLOR_BLUE));
    printf("KS");
    video_setcolor(DEFAULT_COLOR);
    puts(" -> kernel structure");

    video_setcolor(MAKE_COLOR(COLOR_BLACK, COLOR_WHITE));
    printf("PT");
    video_setcolor(DEFAULT_COLOR);
    puts(" -> temporary pointer");

    video_setcolor(MAKE_COLOR(COLOR_BLACK, COLOR_LIGHT_CYAN));
    printf("EX");
    video_setcolor(DEFAULT_COLOR);
    puts(" -> executable file");

    video_setcolor(MAKE_COLOR(COLOR_WHITE, COLOR_RED));
    printf("FS");
    video_setcolor(DEFAULT_COLOR);
    puts(" -> ram file system");
    puts("\n");
    asm("sti");
}

static char name[] = "map";
static char desc[] = "Print out kernel memory map";

static void __attribute__((constructor)) addt_command(void)
{
    register_shell_command(name, (void*) print_memory_blocks, desc);
}

