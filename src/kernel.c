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

void __attribute__((constructor)) handler_initialize(void)
{
    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
}

#endif

extern void (*__init_array_start []) (void);
extern void (*__init_array_end []) (void);

void __init(void)
{
    mem_initialize();   // initialize stage 1 memory manager

    size_t i = __init_array_end - __init_array_start;
    while(i--)
        (*__init_array_start[i])();
}

extern void (*__fini_array_start []) (void);
extern void (*__fini_array_end []) (void);
void __fini(void)
{
    size_t i = __fini_array_end - __fini_array_start;
    while(i--)
        (*__fini_array_start[i])();
}

typedef void (*command_fun_ptr) (char**);

typedef struct command_t {
    char command[256];
    command_fun_ptr function;
} command_t;

void command_clear(char** command)
{
    command+=1;
    video_clear();
}
extern void halt(void);
void command_exit(char** command)
{
    command+=1;
    halt();
}

#define command_list_size 3
command_t command_list[command_list_size];
void __attribute__((constructor)) debug_shell_init(void)
{
    command_list[0] = (command_t) {"clear", command_clear};
    command_list[1] = (command_t) {"exit", command_exit};
    command_list[2] = (command_t) {"halt", command_exit};
}

void debug_command(char* command_string)
{
    char* commandv[256];
    int count = 1;
    commandv[0] = command_string;
    for(int i = 0, size = strlen(command_string);i <= size;i++)
    {
        if(command_string[i] == ' ')
        {
            command_string[i] = 0;
            commandv[count] = command_string + i + 1;
            count++;
        }
    }
    if(commandv[0][0] == 0)
        return;
    else
        for(int i = 0;i < command_list_size;i++)
        {
            if(strcmp(command_list[i].command, commandv[0]) == 0)
            {
                command_list[i].function(commandv);
                return;
            }
        }
        printf("%s: command not found\n", commandv[0]);
}

void debug_shell(void)
{
    cursor_pos_t pos;
    char command_string[512] = "";
    printf("\nMadOS> ");
    while(getch != NULL)
    {
        char c = getch();
        putch(c);
        switch(c)
        {
            case '\b':
                video_cursor_pos(&pos);
                if(pos.x < 7)
                {
                    putch(' ');
                    break;
                }
                else
                {
                    command_string[strlen(command_string) - 1] = 0;
                }
                write((int)stdout, " \b", 2);
                break;
            case '\n':
                debug_command(command_string);
                printf("MadOS> ");
                command_string[0] = 0;
                break;
            default:
                strcat(command_string, (char[]){c, 0});
        }
    }
    panic("\ngetchar", 2);
}
void main(multiboot_uint32_t magic, multiboot_info_t* mbi)
{
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf("This is not multiboot, time for a good probing...\n\n", (unsigned) magic);
        goto skip_multiboot;
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
skip_multiboot: ({}); // labels must be part of a statement
    
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
            putchar('.');
        }
    }
    
    video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_BLACK));
    putch('\n');

    debug_shell();
}
