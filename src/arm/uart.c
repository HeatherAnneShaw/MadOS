
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

#define video_INIT_MESSAGE "\nBooting MadOS v0.1\n\n"

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


enum
{
    // The GPIO registers base address.
    GPIO_BASE = 0x20200000,
 
    // The offsets for reach register.
 
    // Controls actuation of pull up/down to ALL GPIO pins.
    GPPUD = (GPIO_BASE + 0x94),
 
    // Controls actuation of pull up/down for specific GPIO pin.
    GPPUDCLK0 = (GPIO_BASE + 0x98),
 
    // The base address for UART.
    UART0_BASE = 0x1c090000,
 
    // The offsets for reach register for the UART.
    UART0_DR     = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR     = (UART0_BASE + 0x18),
    UART0_ILPR   = (UART0_BASE + 0x20),
    UART0_IBRD   = (UART0_BASE + 0x24),
    UART0_FBRD   = (UART0_BASE + 0x28),
    UART0_LCRH   = (UART0_BASE + 0x2C),
    UART0_CR     = (UART0_BASE + 0x30),
    UART0_IFLS   = (UART0_BASE + 0x34),
    UART0_IMSC   = (UART0_BASE + 0x38),
    UART0_RIS    = (UART0_BASE + 0x3C),
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44),
    UART0_DMACR  = (UART0_BASE + 0x48),
    UART0_ITCR   = (UART0_BASE + 0x80),
    UART0_ITIP   = (UART0_BASE + 0x84),
    UART0_ITOP   = (UART0_BASE + 0x88),
    UART0_TDR    = (UART0_BASE + 0x8C),
};

/* Loop <delay> times in a way that the compiler won't optimize away. */
static inline void delay(int32_t count)
{
	asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
		 : : [count]"r"(count) : "cc");
}

static inline void mmio_write(uint32_t reg, uint32_t data)
{
	*(volatile uint32_t *)reg = data;
}

static inline uint32_t mmio_read(uint32_t reg)
{
	return *(volatile uint32_t *)reg;
}


void uart_init()
{
	// Disable UART0.
	mmio_write(UART0_CR, 0x00000000);
	// Setup the GPIO pin 14 && 15.
 
	// Disable pull up/down for all GPIO pins & delay for 150 cycles.
	mmio_write(GPPUD, 0x00000000);
	delay(150);
 
	// Disable pull up/down for pin 14,15 & delay for 150 cycles.
	mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
	delay(150);
 
	// Write 0 to GPPUDCLK0 to make it take effect.
	mmio_write(GPPUDCLK0, 0x00000000);
 
	// Clear pending interrupts.
	mmio_write(UART0_ICR, 0x7FF);
 
	// Set integer & fractional part of baud rate.
	// Divider = UART_CLOCK/(16 * Baud)
	// Fraction part register = (Fractional part * 64) + 0.5
	// UART_CLOCK = 3000000; Baud = 115200.
 
	// Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
	// Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
	mmio_write(UART0_IBRD, 1);
	mmio_write(UART0_FBRD, 40);
 
	// Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
	mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
 
	// Mask all interrupts.
	mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
	                       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
 
	// Enable UART0, receive & transfer part of UART.
	mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

unsigned char uart_getchar();
extern unsigned char (*getch) (void);
void __attribute__((constructor)) video_initialize()
{
    uart_init();
    getch = uart_getchar;
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

void video_putchar(uint8_t c)
{
    // Wait for UART to become ready to transmit.
	while ( mmio_read(UART0_FR) & (1 << 5) ) { }
	mmio_write(UART0_DR, c);
}

unsigned char uart_getchar()
{
    // Wait for UART to have recieved something.
    while (mmio_read(UART0_FR) & (1 << 4)) { }
    unsigned char c = mmio_read(UART0_DR);
    
    switch(c)
    {
        case 0xd:
            return '\n';
        case 0x7f:
            return '\b';
        default:
            //printf("\n%x\n", c);
            return c;
    }
}

void video_writestring(const char* data, size_t size)
{
    for(size_t i = 0;i < size;i++)
        video_putchar(data[i]);
}


