
#include <stdio.h>
#include <machine.h>

unsigned char kbdus[512] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	'9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, // 29   - Control
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`',   0,	/* 42 Left shift */ '\\', 'z', 'x', 'c', 'v', 'b', 'n','m',
    ',', '.', '/', 0, /* 54 Right shift */ '*', 0,	/* 56 Alt */ ' ',	/* 57 Space bar */
    0, /* Caps lock */ 0, /* 59 - F keys -> */ 0, 0, 0, 0, 0, 0, 0, 0, 0, /* < ... F10 */
    0, /* 69 - Num lock*/ 0, /* Scroll Lock */ 0,	/* Home key */ 0, /* Up Arrow */
    0, /* Page Up */ '-', 0, /* Left Arrow */ 0, 0, /* Right Arrow */ '+', 0, /* 79 - End key*/
    0, /* 80 Down Arrow */ 0, /* 81 Page Down */ 0, /* 82 Insert Key */ 0, /* 83 Delete Key */
    0,   0,   0, 0, /* 87 F11 Key */ 0, /* 88 F12 Key */ 0, /* 90 All other keys are undefined */

    // shift is pressed
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	'(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, // 29   - Control
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '\"', '~',   0,	/* 42 Left shift */ '|', 'Z', 'X', 'C', 'V', 'B', 'N','M',
    '<', '>', '?', 0, /* 54 Right shift */ '*', 0,	/* 56 Alt */ ' ',	/* 57 Space bar */
    0, /* Caps lock */ 0, /* 59 - F keys -> */ 0, 0, 0, 0, 0, 0, 0, 0, 0, /* < ... F10 */
    0, /* 69 - Num lock*/ 0, /* Scroll Lock */ 0,	/* Home key */ 0, /* Up Arrow */
    0, /* Page Up */ '-', 0, /* Left Arrow */ 0, 0, /* Right Arrow */ '+', 0, /* 79 - End key*/
    0, /* Down Arrow */ 0, /* Page Down */ 0, /* Insert Key */ 0, /* Delete Key */
    0,   0,   0, 0, /* F11 Key */ 0, /* F12 Key */ 0, /* All other keys are undefined */
};

bool ctrl = false;
bool shift = false;
bool caps_lock = false;

volatile unsigned char character;
/* Handles the keyboard interrupt */
void keyboard_handler(struct regs *r)
{
    unsigned char scancode;
    /* Read from the keyboard's data buffer */
    scancode = inb(0x60);
    
    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
        if(scancode - 128 == 29)
            ctrl = false;
        else if(scancode - 128 == 42 || scancode - 128 == 54)
            shift = false;
        else if(scancode - 128 == 58)
            caps_lock = ! caps_lock;
    }
    else
    {
        if(ctrl)
        {
            if(kbdus[scancode] == 'c')
                character = 0x03;
        }
        else if((shift || (caps_lock && (kbdus[scancode + 90] >= 'A' && kbdus[scancode + 90] <= 'Z'))))
        {
            if(kbdus[scancode + 90] != 0)
            {
                if(caps_lock && shift && (kbdus[scancode + 90] >= 'A' && kbdus[scancode + 90] <= 'Z'))
                    character = kbdus[scancode];
                else
                    character = kbdus[scancode + 90];
            }
        }
        else if(kbdus[scancode] != 0)
        {
            character = kbdus[scancode];
        }
        else switch(scancode)
        {
            case 29:
                ctrl = true;
                break;
            case 42:
            case 54:
                shift = true;
                break;
            case 58:
                break;
            default:
                printf("\nScancode: %i\n", scancode);
                break;
        }
    }
}

unsigned char keyboard_getch(void)
{
    character = 0;
    irq_install_handler(1, keyboard_handler);
    while(character == 0);
    irq_uninstall_handler(1);
    return character;
}

extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern unsigned char (*getch) (void);
void __attribute__((constructor)) keyboard_install()
{
    getch = keyboard_getch;
}
