
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
#include <mfs.h>


bool is_mfs(mfs_partition_info_t* pi)
{
    char id_test[7];
    memcpy(id_test, pi->mib.magic, 6);
    id_test[6] = 0;
    if(strcmp(id_test, "\xeb\x3c\x90MFS") == 0)
        return true;
    else
        return false;
}

void mfs_load_module(char* name, mfs_partition_info_t* pi)
{
    puts(name);
}

static char name[] = "mfs";
void __attribute__((constructor)) init_mfs()
{
    // set up executable format descriptor
    puts("Registered MFS filesystem format.");
    fs_format_entry_t* mfs = malloc(sizeof(fs_format_entry_t));
    mfs->name = name;
    mfs->is_type = (void*) is_mfs;
    mfs->load_module = (void*) mfs_load_module;

    // set up memory region for format specifier
    mem_entry_t* p = (void*) mfs - sizeof(mem_entry_t);
    p->type = STR;
    register_fs(mfs);
}


/* borrowed from here: https://en.wikipedia.org/wiki/Fletcher%27s_checksum */
uint32_t fletcher32( uint16_t const* data, size_t words )
{
    uint32_t sum1 = 0xffff, sum2 = 0xffff;
    size_t tlen;

    while (words)
    {
        tlen = words >= 359 ? 359 : words;
        words -= tlen;
        do {
            sum2 += sum1 += *data++;
        } while (--tlen);
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }
    /* Second reduction step to reduce sums to 16 bits */
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return sum2 << 16 | sum1;
}










