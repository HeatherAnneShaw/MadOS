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
    int (*fopen) (const char*, const char*);                     // Open a file to a file descriptor
    int (*fseek) (int filedes, long int, int);                   // seek to a byte offset in the file
    int (*fclose) (int filedes);                                 // close a file descriptor
    size_t (*fread) (void*, size_t, size_t, int filedes);        // read from file
    size_t (*fwrite) (const void*, size_t, size_t, int filedes); // write to a file
    int (*creat) (const char* path, mode_t mode);                // create a new file path
    int (*unlink) (const char* pathname);                        // delete a file
    int (*symlink) (const char* path1, const char* path2);       // create a symbolic link
    int (*mkdir) (const char* path, mode_t mode);                // make a new directory
    int (*rmdir) (const char* pathname);                         // remove a directory
    int (*fstat) (int fildes, fileinfo_t* buf);                  // returns a file information structure
    int (*fchmod) (int fildes, int mode);                        // change file attributes
    int (*fchown) (int fildes, uint8_t owner, uint8_t group);    // change file ownership
} fs_entry_t;

extern fs_entry_t* fs_table[MAX_FS_FORMATS];
extern size_t registered_fs_handlers;

extern bool register_fs(fs_entry_t* entry);

typedef struct volume {
    char* label;                    // volume label
    uint8_t media_type;             // type of media
    uint16_t dir_entries;           // max number of directory entries
    fs_entry_t* fs;                 // all the utilities needed to abstract the filesystem
    uint8_t volume_number;          // offset in volume table
} volume_t;

#define MAX_VOLUMES 256
volume_t volume_table[MAX_VOLUMES]; // table of volumes

//////////////////////////////////////////////////////////////////////////////////
// How it all works: (Hopefully I get around to doing this in other files lol)  //
//////////////////////////////////////////////////////////////////////////////////
// Volume names are generated as a volume type prefix, and their volume number  //
// MadOS only supports DOS file permissions right now, will abstract into a     //
// general concept later.                                                       //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////



#endif




