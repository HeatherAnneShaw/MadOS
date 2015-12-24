////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#ifndef __FS_H__
#define __FS_H__

#include <stdlib.h>
#include <machine.h>

#define MAX_FILENAME 255
typedef struct fileinfo {
    char name[MAX_FILENAME];        // file name
    uint8_t owner;                  // file owner
    uint8_t group;                  // file group
    uint32_t attributes;            // file attributes
    uint32_t creation_dt;           // creation date / time
    uint32_t write_dt;              // last write date / time
    uint32_t size;                  // file size
} fileinfo_t;


#define MAX_FS_FORMATS 10
typedef struct fs_entry {
    char* name;                           // Name of filesystem type
    bool (*is_type) (void*);              // FS detection
    bool (*load_module) (char*, void*);   // load as a module

// File IO magic
    int (*open) (uint8_t, void*, const char*, const char*);                      // Open a file to a file descriptor
    int (*creat) (uint8_t, void*, const char* path, mode_t mode);                // create a new file path
    int (*unlink) (uint8_t, void*, const char* pathname);                        // delete a file
    int (*symlink) (uint8_t, void*, const char* path1, const char* path2);       // create a symbolic link
    int (*mkdir) (uint8_t, void*, const char* path, mode_t mode);                // make a new directory
    int (*rmdir) (uint8_t, void*, const char* pathname);                         // remove a directory
} fs_format_entry_t;

extern fs_format_entry_t* fs_format_table[MAX_FS_FORMATS];
extern size_t registered_fs_handlers;

extern bool register_fs(fs_format_entry_t* entry);


//////////////////////////////////////////////////////////////////////////////////
// How it all works: (Hopefully I get around to doing this in other files lol)  //
//////////////////////////////////////////////////////////////////////////////////
// Volume names are generated as a volume type prefix, and their volume number  //
// MadOS only supports DOS file permissions right now, will abstract into a     //
// general concept later.                                                       //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#define MAX_PATH 256

// vfs volume type, used for mount_data
#define RAMDISK      1

typedef struct __attribute__((packed, aligned(1))) ramdisk {
    uint32_t ramdisk_start;    // where does the ramdisk begin?
} ramdisk_t;

typedef struct __attribute__((packed, aligned(1))) volume {
    char label[12];         // volume label
    uint8_t type;           // vfs volume type
    uint32_t mount_data;       // pointer to struct needed to feed filesystem functions
    fs_format_entry_t* fs;  // all the utilities needed to abstract the filesystem
} volume_t;


typedef struct __attribute__((packed, aligned(1))) vfs_node {
    char vpath[MAX_PATH];   // node virtual file path
    uint8_t attributes;     // node file attributes
    uint8_t ownership;      // upper is group, lower is owner
    volume_t volume;        // the volume for this vnode
} vfs_node_t;

#define MAX_VFS_NODES 25
extern vfs_node_t vfs_node_table[MAX_VFS_NODES];

typedef struct __attribute__((packed, aligned(1))) open_filedesc {
    size_t offset;          // file offset for seek
    char path[MAX_PATH];    // absolute path to open file on volume
    volume_t* volume;       // the volume the file is contained in
} open_filedesc_t;

#define MAX_OPEN_FILES 256
open_filedesc_t open_file_table[MAX_OPEN_FILES];

//////////////////////////////////////////////////////////////
//            VIRTUAL FILESYSTEM NOTES:                     //
//////////////////////////////////////////////////////////////
//                                                          //
// Virtual file attributes conform to MFS attributes        //
//                                                          //
// Virtual nodes may be created in any existing directory,  //
// with files in the underlying filesystem (if any) being   //
// being ignored if a virtual file of the same name exists  //
// within the virtual filesystem                            //
//                                                          //
// Virtual device files may be written to like any other    //
// file, and may also contain runtime attributes defined    //
// in /etc/boot.cfg                                         //
//                                                          //
// Virtual device files may not be links                    //
//                                                          //
// Any file may be mounted as a virtual node as long as it  //
// contains a valid filesystem                              //
//                                                          //
//////////////////////////////////////////////////////////////

extern bool vfs_mknode(const char* vpath, uint8_t attributes, uint8_t ownership, volume_t* volume);


#endif













