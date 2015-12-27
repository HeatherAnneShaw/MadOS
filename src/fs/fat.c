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
    if(strcmp(id_test, "FAT12") == 0)
        return true;
    else
        return false;
}

void fat12_load_module(char* name, fat12_partition_info_t* pi)
{
    volume_t* volume = malloc(sizeof(volume_t));
    volume->type = RAMDISK;
    memcpy(volume->label, pi->ebr.label, 10);
    volume->mount_data = (uint32_t) malloc(sizeof(ramdisk_t));
    // set up memory region for format specifier
    mem_entry_t* p = (void*) volume->mount_data - sizeof(mem_entry_t);
    p->type = FS;
    ((ramdisk_t*)(volume->mount_data))->ramdisk_start = (uint32_t) &pi;
    if(vfs_mknode("/", 0/* change all of the 0'2 */, 0, volume))
        printf("Loaded Volume: %s, With Label: %s\n", name, volume->label);
    else
        free(volume);
}


/*
    int (*open) (uint8_t, void*, const char*, const char*);                     // Open a file to a file descriptor
    int (*creat) (uint8_t, void*, const char* path, mode_t mode);                // create a new file path
    int (*unlink) (uint8_t, void*, const char* pathname);                        // delete a file
    int (*symlink) (uint8_t, void*, const char* path1, const char* path2);       // create a symbolic link
    int (*mkdir) (uint8_t, void*, const char* path, mode_t mode);                // make a new directory
    int (*rmdir) (uint8_t, void*, const char* pathname);                         // remove a directory
*/




static char name[] = "FAT12";
void __attribute__((constructor)) init_fat12()
{
    // set up executable format descriptor
    puts("Registered FAT12 filesystem format.");
    fs_format_entry_t* fat12 = malloc(sizeof(fs_format_entry_t));
    fat12->name = name;
    fat12->is_type = (void*) is_fat12;
    fat12->load_module = (void*) fat12_load_module;
    //fat12->open = (void*) fat12_open;
    // set up memory region for format specifier
    mem_entry_t* p = (void*) fat12 - sizeof(mem_entry_t);
    p->type = STR;
    register_fs(fat12);
}








