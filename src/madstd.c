////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <video.h>
#include <fs.h>

///////////////////////// FILE IO //////////////////////////////////////////////

int mount(const char* dfile, const char* vpath);

int chroot(const char* path);

ssize_t write(int fildes, const void* buf, size_t nbyte)
{
    switch(fildes)
    {
        default:
            video_writestring(buf, nbyte);
            break;
    }
    return nbyte;
}

ssize_t read(int fildes, void* buf, size_t nbyte)
{
    switch(fildes)
    {
        case (int) stdin:
        buf=buf;
        break;
    }
    return nbyte;
}


