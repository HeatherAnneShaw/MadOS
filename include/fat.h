////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#ifndef __FAT_H__
#define __FAT_H__

#include <stdint.h>
#include <machine.h>

typedef struct __attribute__((packed, aligned(1))) bpb {
    char jmp_code[3];
    char oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fats;
    uint16_t dir_entries;
    uint16_t logical_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t large_sectors;
} bpb_t;

typedef struct __attribute__((packed, aligned(1))) ebr {
    uint8_t drive;
    uint8_t rflags;
    uint8_t signiture;
    uint32_t serial;
    char label[11];
    char identifier[8];
    char boot_code[448];
    uint16_t boot_sign;
} ebr_t;

typedef struct __attribute__((packed, aligned(1))) fat12_partition_info {
// see fat12.s for an implementation
// in assembly for visualization :D

    bpb_t bpb;
    ebr_t ebr;
    char data[];
} fat12_partition_info_t;

typedef struct __attribute__((packed, aligned(1))) fat_dir_entry {
    char name[11];
    uint8_t attributes;
    uint16_t reserved;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t caddr_high;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t caddr_low;
    uint32_t size;
} fat_dir_entry_t;

const uint8_t fat12_format_stub[122] = {
  0xEB, 0x3C, 0x90, 0x4D, 0x61, 0x64, 0x4F, 0x53, 0x76, 0x30,
  0x31, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0x37,
  0x13, 0x00, 0x00, 0x4D, 0x61, 0x64, 0x4F, 0x53, 0x20, 0x44,
  0x69, 0x73, 0x6B, 0x20, 0x46, 0x41, 0x54, 0x31, 0x32, 0x20,
  0x20, 0x20, 0x31, 0xC0, 0x8E, 0xD8, 0x8E, 0xC0, 0x8E, 0xD0,
  0xBC, 0x7A, 0x8C, 0x8D, 0x36, 0x5E, 0x7C, 0xE8, 0x02, 0x00,
  0xEB, 0xFE, 0xAC, 0x08, 0xC0, 0x74, 0x06, 0xB4, 0x0E, 0xCD,
  0x10, 0xEB, 0xF5, 0xC3, 0x4D, 0x61, 0x64, 0x4F, 0x53, 0x20,
  0x64, 0x69, 0x73, 0x6B, 0x20, 0x6E, 0x6F, 0x74, 0x20, 0x62,
  0x6F, 0x6F, 0x74, 0x61, 0x62, 0x6C, 0x65, 0x2E, 0x2E, 0x2E,
  0x0D, 0x0A
};


/////////////////////////////////////////////
// FAT12 entries                           //
/////////////////////////////////////////////
// an entry is 3 nibbles:                  //
//   000         -> unused                 //
//   0xFF0-0xFF6 -> reserved cluster       //
//   0xFF7       -> bad cluster            //
//   0xFF8-0xFFF -> last cluster in file   //
//   ***         -> next data cluster      //
/////////////////////////////////////////////

typedef char fat12_t[];
#define GET_FAT12_ENTRY(n, fat)\
    (n % 2 == 0 ? fat[((3 * n) / 2)] | (fat[1 + (3 * n) / 2] & 0x0F) << 8 : \
    (fat[(3 * n) / 2] & 0xF0) >> 4 | fat[1 + (3 * n) / 2] << 4)


#endif






