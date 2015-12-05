////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#ifndef STDLIB_H
#define STDLIB_H

// libc stuff
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

// STUFF
#include <madmath.h>
#include <madmacro.h>


typedef __SIZE_TYPE__ size_t;
typedef __SIZE_TYPE__ ssize_t;

typedef enum { false, true } bool;

typedef struct mem_entry{
    void* ptr;
    size_t size;
    bool free;
} mem_entry_t;


#define null 0

#define lambda(type, body) ({ type lambda body lambda; })

#define STR_MAX_LENGTH	1024

#define stdin (FILE*) 0
#define stdout (FILE*) 1
#define stderr (FILE*) 2

extern void* malloc_early(size_t size);
typedef void* (malloc_t) (size_t);
malloc_t* malloc;
extern void mem_initialize(void);

extern void free(void* ptr);
extern void* calloc(size_t nmemb, size_t size);
extern void* realloc(void* ptr, size_t size);


extern void srand(unsigned int __seed);
extern int rand(void);

extern long strtol(const char* str);
#define atoi(str) (int) strtol(str)

#include <madstd.h>
#include <stdio.h>


// for really bad debugging, use this lol
//#define DEBUG

#endif
