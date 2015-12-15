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

#if !defined(__arm__)

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

#endif

