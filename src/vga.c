
#include <vga.h>
#include <string.h>

size_t vga_row = 0;
size_t vga_column = 0;
uint8_t vga_color;

void vga_initialize()
{
	vga_setcolor(MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK));
	vga_clear();
}

void vga_clear(void)
{
    for(size_t y = 0; y < VGA_HEIGHT; y++)
    {
		for(size_t x = 0; x < VGA_WIDTH; x++)
        {
			const size_t index = y * VGA_WIDTH + x;
			VGA_BUFFER[index] = MAKE_VGAENTRY(' ', vga_color);
		}
	}
    update_cursor(0, 0);
}

void update_cursor(int x, int y)
{
    unsigned short position=(y*80) + x;
    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}
 
void vga_setcolor(uint8_t color)
{
	vga_color = color;
}
 
void vga_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	VGA_BUFFER[index] = MAKE_VGAENTRY(c, color);
    update_cursor(x+1, y);
}
 
void vga_scroll(void)
{
    for(int y = 0;y < VGA_HEIGHT;y++)
    {
        for(int x = 0;x < VGA_WIDTH;x++)
        {
            VGA_BUFFER[y * VGA_WIDTH + x] = VGA_BUFFER[(y + 1) * VGA_WIDTH + x];
        }
    }
    vga_row = VGA_HEIGHT - 1;
    vga_column = 0;
}

void vga_putchar(char c)
{
    switch(c)
    {
        case '\r':
            vga_column = 0;
            break;
        case '\n':
            vga_column = 0;
            ++vga_row;
            break;
        case '\t':
            vga_column += vga_column % 4;
            break;
        default:
            vga_putentryat(c, vga_color, vga_column, vga_row);
            ++vga_column;
            break;
    }
    if(vga_column == VGA_WIDTH)
        vga_column = 0;
    if(vga_row == VGA_HEIGHT)
        vga_scroll();
    update_cursor(vga_column, vga_row);
    return;
}
 
void vga_writestring(const char* data)
{
	size_t datalen = strlen(data);
	for(size_t i = 0;i < datalen;i++)
        vga_putchar(data[i]);
}




