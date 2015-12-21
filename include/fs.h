////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#ifndef __RAMDISK_H__
#define __RAMDISK_H__

#include <stdlib.h>
#include <machine.h>

#define MAX_FS_FORMATS 10
typedef struct fs_entry {
    char* name;
    bool (*is_type) (void*);
    bool (*load_module) (char*, void*);
    // add read, write, info, etc ...
} fs_entry_t;

extern fs_entry_t* fs_table[MAX_FS_FORMATS];
extern size_t registered_fs_handlers;

extern bool register_fs(fs_entry_t* entry);

#define MAX_FILENAME 255
typedef struct fileinfo {
    char name[MAX_FILENAME];    // pointer to filename
    uint32_t attributes;        // file attributes
    uint32_t creation_dt;       // creation date / time
    uint32_t write_dt;          // last write date / time
    uint32_t size;              // file size
} fileinfo_t;

typedef struct volume {
    char* label;                // volume label
    uint8_t media_type;         // type of media
    uint16_t dir_entries;       // max number of directory entries
    fs_entry_t* fs;             // all the utilities needed to abstract the filesystem
} volume_t;



#endif




