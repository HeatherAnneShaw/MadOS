#ifndef __MADSTD_H__
#define __MADSTD_H__

#include <stdlib.h>

extern int open(const char* vpath);
extern ssize_t write(int fildes, const void* buf, size_t nbyte);
extern ssize_t read(int fildes, void* buf, size_t nbyte);

#endif
