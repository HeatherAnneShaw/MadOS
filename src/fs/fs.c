////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <fs.h>

fs_entry_t* fs_table[MAX_FS_FORMATS] = { NULL };
size_t registered_fs_handlers = 0;

bool register_fs(fs_entry_t* entry)
{
    if(registered_fs_handlers >= MAX_FS_FORMATS) return false;
    fs_table[registered_fs_handlers++] = entry;
    return true;
}


