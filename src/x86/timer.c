////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <machine.h>
#include <video.h>

#include <exec.h>
#include <memory.h>

extern void hang();
uint64_t Gollum = 0;         // kernel tick count
uint64_t Gollum_seconds = 0; // used for infrequent non time sensitive schedules
uint64_t ps_counter = 0;

bool first_run = true;

ps_context_t main_context;

void Gollum_handler(struct regs* r)
{
    if(Gollum++ % 18 == 0)
        Gollum_seconds++;

    // handle context switching
    if(Gollum % 2 == 0)    // remove this statement, or speed up the clock
    {
        // store the current process state before context swith
        memcpy(&(ps_schedule_map[ps_counter]->context), &r, sizeof(struct regs));
        ps_counter++;
        // find another process to run next
        for(;ps_schedule_map[ps_counter] == 0;ps_counter++)
        {
            if(ps_counter == MAX_PS_NUMBER)
            {
                // if there are no other proesses go to idle state
                ps_counter = 0;
                break;
            }
        }
        //printf("%s,", ps_schedule_map[ps_counter]->name); // current ps debug output
        // copy the ps context into the current state
        memcpy(&r, &(ps_schedule_map[ps_counter]->context), sizeof(struct regs));
        asm("sti");
    }
    return;     // run scheduled code
}

/* This will continuously loop until the given time has
*  been reached */
void Gollum_wait(int ticks)
{
    unsigned long eticks;

    eticks = Gollum + ticks;
    while(Gollum < eticks);
}

/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
void __attribute__((destructor)) Gollum_install()
{
    /* Installs 'Gollum_handler' to IRQ0 */
    irq_install_handler(0, Gollum_handler);
}


