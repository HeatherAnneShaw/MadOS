
#include <string.h>

#include <module.h>
#include <video.h>

#include "./machine.h"

// Hide cursor
#define HIDE_CURSOR     "\033[?25l"

//Show cursor
#define SHOW_CURSOR     "\033[?25h"

// Cursor position
#define CURSOR(x, y) printf("\033[%i;%iH\n", y, x)

// Clear
#define CLEAR           "\033[2J"

// Reset
#define RESET		"\033[0m"

// Regular Colors
#define BLACK		"\033[030m"
#define RED	    	"\033[031m"
#define GREEN		"\033[032m"
#define YELLOW		"\033[033m"
#define BLUE		"\033[034m"
#define PURPLE		"\033[035m"
#define CYAN		"\033[036m"
#define WHITE		"\033[037m"

// High Intensity
#define IBLACK		"\033[090m"
#define IRED		"\033[091m"
#define IGREEN		"\033[092m"
#define IYELLOW		"\033[093m"
#define IBLUE		"\033[094m"
#define IPURPLE		"\033[095m"
#define ICYAN		"\033[096m"
#define IWHITE		"\033[097m"

///////////////////////////////////

// Background
#define BGBLACK		"\033[40m"
#define BGRED		"\033[41m"
#define BGGREEN		"\033[42m"
#define BGYELLOW	"\033[43m"
#define BGBLUE		"\033[44m"
#define BGPURPLE	"\033[45m"
#define BGCYAN		"\033[46m"
#define BGWHITE		"\033[47m"

// High Intensity backgrounds
#define BGIBLACK	"\033[0100m"
#define BGIRED		"\033[0101m"
#define BGIGREEN	"\033[0102m"
#define BGIYELLOW	"\033[0103m"
#define BGIBLUE		"\033[0104m"
#define BGIPURPLE	"\033[0105m"
#define BGICYAN		"\033[0106m"
#define BGIWHITE	"\033[0107m"

char* color_table[] = {
    RESET,
    BLUE,
    GREEN,
    CYAN,
    RED,
    PURPLE,
    YELLOW,
    WHITE,
    IBLACK,
    IBLUE,
    IGREEN,
    ICYAN,
    IRED,
    IPURPLE,
    IYELLOW,
    IWHITE,
    RESET,
    BGBLUE,
    BGGREEN,
    BGCYAN,
    BGRED,
    BGPURPLE,
    BGYELLOW,
    BGWHITE,
    BGIBLACK,
    BGIBLUE,
    BGIGREEN,
    BGICYAN,
    BGIRED,
    BGIPURPLE,
    BGIYELLOW,
    BGIWHITE
};

#define video_INIT_MESSAGE "\nBooting MadOS v0.1\n\n"
void __attribute__((constructor)) video_initialize()
{
    video_clear();
    video_writestring(GREEN, strlen(GREEN));
    video_writestring(video_INIT_MESSAGE, strlen(video_INIT_MESSAGE));
    video_writestring(RESET, strlen(RESET));
}

void video_setcolor(uint8_t color)
{
    video_writestring(color_table[(color >> 4) + 16], strlen(color_table[(color >> 4) + 16]));
    video_writestring(color_table[color & 0xF], strlen(color_table[color & 0xF]));
}

void video_update_cursor(int x, int y)
{
    CURSOR(x, y);
}

void video_clear(void)
{
    video_update_cursor(0, 0);
    video_writestring(CLEAR, strlen(CLEAR));
}

#define UART0_BASE 0x1c090000
void video_putchar(uint8_t c)
{
    *(volatile uint32_t *)(UART0_BASE) = c;
    return;
}
 
void video_writestring(const char* data, size_t size)
{
    for(size_t i = 0;i < size;i++)
        video_putchar(data[i]);
}


