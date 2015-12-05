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

#include <video.h>

#if !defined(__i386__)
#error "This is for x86 dude, wrong architecture..."
#endif

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

void main(multiboot_uint32_t magic, multiboot_info_t* mbi)
{
    mem_initialize();
    video_initialize();
    
    printf("Booting MadOS v0.1\n\n");
    video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_BLACK));
    
    
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf("Invalid boot magic: 0x%x\n", (unsigned) magic);
        return;
    }
    
    printf("flags = 0b%b\n", (unsigned) mbi->flags);
    
    if(CHECK_FLAG(mbi->flags, 0))
        printf("mem_lower = %uKB, mem_upper = %uKB\n",
            (unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);
     
    // Is boot_device valid? 
    if(CHECK_FLAG(mbi->flags, 1))
        printf("boot_device = 0x%x\n", (unsigned) mbi->boot_device);
        
    // Is the command line passed? 
    if(CHECK_FLAG(mbi->flags, 2))
        printf("cmdline = %s\n", (char *) mbi->cmdline);
     
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
            " addr = 0x%x, shndx = 0x%x\n",
            (unsigned) multiboot_elf_sec->num, (unsigned) multiboot_elf_sec->size,
            (unsigned) multiboot_elf_sec->addr, (unsigned) multiboot_elf_sec->shndx);
    }
    
    char* p;
    unsigned int counter = 0;
    puts("Allocating / freeing 80MB to test memory manager stability:\n");
    while(counter < 1024 * 80)
    {
        p = malloc(1024);
        memset(p, 'X', 1024);
        free(p);
        if(++counter % 1024 == 0)
        {
            video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_LIGHT_GREEN));
            putchar(' ');
        }
    }
    video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_BLACK));
    puts("\nIf there was no kernel panic, I'm awesome sauce.\n\nNow lets run out of memory :D");
    while(true) malloc(1024);
}
