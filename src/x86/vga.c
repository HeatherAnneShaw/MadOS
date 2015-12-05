
#include <string.h>

#include <module.h>
#include <video.h>

#include "./machine.h"

#define video_INIT_MESSAGE "Booting MadOS v0.1\n\n"
#define video_BUFFER ((uint16_t*) 0xB8000)
#define video_WIDTH 80
#define video_HEIGHT 25

size_t video_row = 0;
size_t video_column = 0;
uint8_t video_color;

void __attribute__((constructor)) video_initialize()
{
    video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK));
    video_clear();
    
    video_writestring(video_INIT_MESSAGE, strlen(video_INIT_MESSAGE));
    video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_BLACK));
}

void video_clear(void)
{
    for(size_t y = 0; y < video_HEIGHT; y++)
    {
        for(size_t x = 0; x < video_WIDTH; x++)
        {
            const size_t index = y * video_WIDTH + x;
            video_BUFFER[index] = MAKE_VGAENTRY(' ', video_color);
        }
    }
    video_update_cursor(0, 0);
}

void video_update_cursor(int x, int y)
{
    unsigned short position=(y*80) + x;
    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}
 
void video_setcolor(uint8_t color)
{
    video_color = color;
}
 
void video_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * video_WIDTH + x;
    video_BUFFER[index] = MAKE_VGAENTRY(c, color);
}
 
void video_scroll(void)
{
    for(int y = 0;y < video_HEIGHT;y++)
    {
        for(int x = 0;x < video_WIDTH;x++)
        {
            video_BUFFER[y * video_WIDTH + x] = video_BUFFER[(y + 1) * video_WIDTH + x];
        }
    }
    video_row = video_HEIGHT - 1;
    video_column = 0;
}

void video_putchar(char c)
{
    switch(c)
    {
        case '\r':
            video_column = 0;
            break;
        case '\n':
            video_column = 0;
            ++video_row;
            break;
        case '\t':
            video_column += ((video_column + 1) % 4);
            break;
        default:
            video_putentryat(c, video_color, video_column, video_row);
            ++video_column;
            break;
    }
    if(video_column >= video_WIDTH)
    {
        video_column = 0;
        ++video_row;
    }
    if(video_row >= video_HEIGHT)
        video_scroll();
    video_update_cursor(video_column, video_row);
    return;
}
 
void video_writestring(const char* data, size_t size)
{
    for(size_t i = 0;i < size;i++)
        video_putchar(data[i]);
}


