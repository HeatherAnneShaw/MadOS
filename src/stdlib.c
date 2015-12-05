////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///!!!!!!!!!!!!!!!!!!!!!!!!!!! MEMORY AND I/O !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/******************************************************************
    This malloc / free implementation is just temporary so that
    I may have stone age memory management while I work on other
    things.
*******************************************************************/
#define MEM_EARLY_SIZE 1024 * 3
mem_entry_t MEM_FLAT_TABLE[MEM_EARLY_SIZE];
mem_entry_t* MEM_FLAT_TABLE_TOP = MEM_FLAT_TABLE;

char MEM_KLUDGE[MEM_EARLY_SIZE];
char* MEM_KLUDGE_END = MEM_KLUDGE + MEM_EARLY_SIZE;
char* MEM_PTR = MEM_KLUDGE;

void mem_initialize(void)
{
    for(unsigned i = 0;i < MEM_EARLY_SIZE;i++)
    {
        MEM_KLUDGE[i] = 0;
        (MEM_FLAT_TABLE+i)->ptr = NULL;
        (MEM_FLAT_TABLE+i)->size = 0;
    }
}

malloc_t* malloc = malloc_early;
void* malloc_early(size_t size)
{
    MEM_PTR += size;
    if(MEM_PTR >= MEM_KLUDGE_END)
        panic("malloc", 0);
    
    MEM_FLAT_TABLE_TOP->ptr = MEM_PTR - size;
    MEM_FLAT_TABLE_TOP->size = size;
    MEM_FLAT_TABLE_TOP += 1;
    return MEM_PTR - size;
}

void free(void* ptr)
{
    int i;
    for(i = 0;i <= MEM_EARLY_SIZE;i++)
    {
        if((MEM_FLAT_TABLE+i)->ptr == ptr)
        {
            (MEM_FLAT_TABLE+i)->ptr = NULL;
            if(ptr + (MEM_FLAT_TABLE+i)->size == MEM_PTR)
            {
                MEM_PTR -= (MEM_FLAT_TABLE+i)->size;
                (MEM_FLAT_TABLE+i)->size = 0;
            }
            break;
        }
    }
    if(i == MEM_EARLY_SIZE) return;
    if((MEM_FLAT_TABLE + i) == (MEM_FLAT_TABLE_TOP - 1))
    {
        MEM_FLAT_TABLE_TOP -= 1;
    }
}

void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);


///!!!!!!!!!!!!!!!!!!!!!!!!!!! PRNG FUNCTIONS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

unsigned int __prng_seed;
void srand(unsigned int __seed)
{
    __prng_seed = __seed;
}

int rand(void)
{
    __prng_seed ^= RSL(__prng_seed, 13);
    __prng_seed ^= RSR(__prng_seed, 9);
    __prng_seed ^= RSR(__prng_seed, 7);
    return __prng_seed;
}


long strtol(const char* s)
{
    bool sign = false;
    long i, r, base = 10;
    if(s[1] == 'x')
    {
        s+=2;
        base = 16;
    }
    else if(s[1] == 'b')
    {
        s+=2;
        base = 2;
    }
    
    if(s[0] == '-')
    {
        s+=1;
        sign = true;
    }
    
    for (i = 0, r = 0;s[i] != '\0';++i)
        r = r*base + s[i] - (s[i] >= '0' && s[i] <= '9' ? '0' :
                s[i] >= 'A' && s[i] <= 'Z' ? ('A' - 10) :  ('a' - 10));
    return (sign ? -r : r);
}



