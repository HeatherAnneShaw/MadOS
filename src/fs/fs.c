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

fs_format_entry_t* fs_format_table[MAX_FS_FORMATS] = { NULL };
size_t registered_fs_handlers = 0;

bool register_fs(fs_format_entry_t* entry)
{
    if(registered_fs_handlers >= MAX_FS_FORMATS) return false;
    fs_format_table[registered_fs_handlers++] = entry;
    return true;
}

// VFS
vfs_node_t vfs_node_table[MAX_VFS_NODES];
bool vfs_mknode(const char* vpath, uint8_t attributes, uint8_t ownership, volume_t* volume)
{
    int i;
    for(i = 0;vfs_node_table[i].vpath[0] != 0;i++)
    {
        if(i > MAX_VFS_NODES)
        {
            panic("vfs_mknod", 3);
            return false;
        }
    }
    vfs_node_t* tnode = malloc(sizeof(vfs_node_t));
    
    strcpy(tnode->vpath, vpath);
    tnode->attributes = attributes;
    tnode->ownership = ownership;
    memcpy(&(tnode->volume), volume, sizeof(volume_t));
    // set up memory region for format specifier
    mem_entry_t* p = (void*) volume - sizeof(mem_entry_t);
    p->type = STR;
    memcpy(&(vfs_node_table[i]), tnode, sizeof(vfs_node_t));
    free(tnode);
    return true;
}

void vfs_freenode(vfs_node_t* node)
{
    free((void*) node->volume.mount_data);
    free(&(node->volume));
    free(node);
}

void __attribute__((constructor)) init_vfs(void)
{
    memset(vfs_node_table, 0, sizeof(vfs_node_table));
    puts("VFS initialized");
}






