////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include <memory.h>
#include <fs.h>
#include <fat.h>


bool is_fat12(fat12_partition_info_t* pi)
{
    char id_test[6];
    memcpy(id_test, pi->ebr.identifier, 5);
    id_test[5] = 0;
    //puts(id_test);
    if(strcmp(id_test, "FAT12") == 0)
        return true;
    else
        return false;
}

void fat12_load_module(char* name, fat12_partition_info_t* pi)
{
    //printf("Mounted: %s on %s\n", name, /* internal disk name */);
}

static char name[] = "FAT12";
void __attribute__((constructor)) init_fat12()
{
    // set up executable format descriptor
    puts("Registered FAT12 filesystem format.");
    fs_entry_t* fat12 = malloc(sizeof(fs_entry_t));
    fat12->name = name;
    fat12->is_type = (void*) is_fat12;
    fat12->load_module = (void*) fat12_load_module;

    // set up memory region for format specifier
    mem_entry_t* p = (void*) fat12 - sizeof(mem_entry_t);
    p->type = STR;
    register_fs(fat12);
}


