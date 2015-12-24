////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#ifndef __MFS_H__
#define __MFS_H__

#include <stdint.h>
#include <machine.h>

///////////////////////////////////////////
// FEATURES:                             //
///////////////////////////////////////////
// > root directory works like any other //
//   file in terms of the superblock     //
// > large volume support                //
// > journaling                          //
// > support for group / owner and       //
//   attributes                          //
// > made for easy implementation, with  //
//   minimal cost in features            //
//                                       //
///////////////////////////////////////////

// FS FLAGS
#define CHECKSUMS 1
// ... reserved

typedef struct __attribute__((packed, aligned(1))) mib {
    char magic[6];              // always equal to "\xeb\x3c\x90MFS"
    uint16_t bytes_per_sector;  // valid values are: 512, 1024, 2048, 4096
    uint8_t sectors_per_block;  // keep this small if you have a lot of small files
    uint8_t journals;           // number of extra journal backups
    uint32_t max_files;         // Maximum number of files this volume can facilitate
    char reserved[22];          // reserved

    uint8_t drive;              // BIOS drive number
    uint8_t flags;              // flags for checksums, and more
    uint8_t signiture;          // always equal to 0
    uint32_t serial;            // volume serial number
    char label[11];             // volume label
    char identifier[8];         // always equal to "MFS   01" where 0 is the major, and 1 is the minor version
    char boot_code[448];        // place to put bootloaders
    uint16_t boot_sign;         // equal to 0xAA55 when marking the partition bootable
} mib_t;


typedef struct __attribute__((packed, aligned(1))) mfs_partition_info {
    bpb_t mib;
    char blocks[];
    // first two data blocks are reserved for superblock, and journal
} mfs_partition_info_t;

// file attribute flags
#define DIRECTORY 1
#define LINK     (1 << 1)
#define SYSTEM   (1 << 2)
#define HIDDEN   (1 << 3)
#define READ     (1 << 4)
#define WRITE    (1 << 5)
#define EXECUTE  (1 << 6)
// ... reserved


typedef struct __attribute__((packed, aligned(1))) mfs_dir_entry {
    char name[14];              // filename, null terminated, max filesize is actually 13
    uint8_t attributes;         // file attribute flags
    uint8_t ownership;          // upper is group, lower is owner
    uint32_t creation_time;     // unix timestamp for file creation
    uint32_t access_time;       // unix timestamp for last file access
    uint32_t first_block;       // first block of data
    uint32_t checksum;          // file checksum for disk checking, currently fletcher32, updated
                                // upon file close, only used if mib->flags & CHECKSUMS
    uint32_t size;              // file size
} mfs_dir_entry_t;


typedef struct __attribute__((packed, aligned(1))) mfs_dir_entry_link {
    char name[14];              // filename, null terminated, max filesize is actually 13
    uint8_t rattributes;        // file attribute flags for the fs walker
    uint8_t reserved;
    uint32_t dir_block;         // block that contains the directory entry
    uint32_t dir_offset;        // offset from the beginning directory block
    uint32_t reserved2;
    uint64_t reserved3;
} mfs_dir_entry_link_t;


struct root_directory {
    uint8_t attributes;          // attribute flags
    uint8_t ownership;           // upper is group, lower is owner
    uint32_t creation_time;      // unix timestamp for creation
    uint32_t access_time;        // unix timestamp for last access
    uint64_t size;               // file size

    mfs_dir_entry_t first_dir[]; // first directory entry
} root_directory_t;


// MFS superblock flags:
#define FREE_BLOCK 0
#define RESERVED_BLOCK 1
// anything else is a block address

typedef uint32_t mfs_superblock_t[]; // right now we can address up to a little over a terabyte of 512 byte blocks

#endif














