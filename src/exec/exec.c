////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <exec.h>

exec_entry_t* exec_table[MAX_EXEC_FORMATS] = { NULL };
size_t registered_exec_handlers = 0;

bool register_exec(exec_entry_t* entry)
{
    if(registered_exec_handlers >= MAX_EXEC_FORMATS) return false;
    exec_table[registered_exec_handlers++] = entry;
    return true;
}



