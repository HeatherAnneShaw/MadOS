/////////////////////////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <stddef.h>
#include <stdint.h>

#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GREY 7
#define COLOR_DARK_GREY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_LIGHT_BROWN 14
#define COLOR_WHITE 15

#define MAKE_COLOR(fg, bg) (fg | bg << 4)
#define MAKE_VGAENTRY(c, color) ((uint16_t) c | (uint16_t) color << 8)

#define DEFAULT_BG_COLOR COLOR_BLACK
#define DEFAULT_FG_COLOR COLOR_LIGHT_GREY
#define DEFAULT_COLOR MAKE_COLOR(DEFAULT_FG_COLOR, DEFAULT_BG_COLOR)

typedef struct cursor_pos {
    int x;
    int y;
} cursor_pos_t;


extern void video_clear(void);
extern void video_update_cursor(int x, int y);
extern void video_cursor_pos(cursor_pos_t* st);
extern void video_setcolor(uint8_t color);
extern void video_putchar(uint8_t c);
extern void video_writestring(const char* data, size_t size);

#endif
