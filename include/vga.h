
#ifndef __VGA_H__
#define __VGA_H__

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

#define VGA_BUFFER ((uint16_t*) 0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define MAKE_COLOR(fg, bg) (fg | bg << 4)
#define MAKE_VGAENTRY(c, color) ((uint16_t) c | (uint16_t) color << 8)

extern void vga_writestring(const char* data);
extern void vga_setcolor(uint8_t color);
extern void update_cursor(int x, int y);
extern void vga_clear(void);
extern void vga_initialize(void);
extern void vga_putchar(char c);

#endif
