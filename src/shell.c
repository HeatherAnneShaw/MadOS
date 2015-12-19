////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////

#include <video.h>
#include <stdio.h>
#include <string.h>
#include <madmath.h>
#include <shell.h>

#include<exec.h>
#include<memory.h>

ps_context_t* shell_context;
extern uint32_t ps_counter;
void command_clear(void)
{
    video_clear();
    putch('\n');
}

void command_exit(void)
{
    printf("< shell terminated >\n");
    exec_end_schedule(shell_context);
}

void command_echo(int count, char** argv)
{
    for(int i = 1;i < count;i++)
    {
        if(i == count - 1)
            printf("%s", argv[i]);
        else
            printf("%s ", argv[i]);
    }
    puts("");
}

int command_list_size = 0;
#define MAX_COMMAND_LIST_SIZE 50
command_t command_list[MAX_COMMAND_LIST_SIZE];
bool register_shell_command(char* command, command_fun_ptr function, char* description)
{
    if(command_list_size == MAX_COMMAND_LIST_SIZE) return false;
    command_list[command_list_size++] = (command_t) { command, function, description };
    return true;
}


void debug_command(char* command_string)
{
    char* commandv[MAX_COMMAND_LINE_VECTOR_LENGTH];
    memset(commandv, 0, sizeof(commandv));

    int count = 1;
    commandv[0] = command_string;

    for(int i = 0, size = strlen(command_string);i <= size;i++)
    {
        if(command_string[i] == ' ')
        {
            command_string[i] = 0;
            commandv[count] = command_string + i + 1;
            count++;
        }
    }
    if(commandv[0][0] == 0)
        return;
    else
    {
        for(int i = 0;i < command_list_size;i++)
        {
            if(strcmp(command_list[i].command, commandv[0]) == 0)
            {
                command_list[i].function(count, commandv);
                return;
            }
            // if in file path
            else if(strcmp(commandv[0], "help") == 0 || commandv[0][0] == '?')
            {
                if(strcmp(command_list[i].command, commandv[1]) == 0)
                {
                    printf("%s: %s\n\n", commandv[1], command_list[i].description);
                    return;
                }
                else if(commandv[1] == NULL)
                {
                    printf("%s \t\t%s\n", command_list[i].command, command_list[i].description);
                }
            }
        }
        if(strcmp(commandv[0], "help") != 0 && commandv[0][0] != '?')
            printf("%s: command not found\n", commandv[0]);
        putch('\n');
    }
}

#if defined(__arm__)
#define PROMPT "\033[092m[\033[037mMadOS\033[092m]\033[0m "
#else
#define PROMPT "[MadOS] "
#endif
void debug_shell(void)
{
    cursor_pos_t pos;
    char command_string[MAX_COMMAND_LINE_LENGTH] = "";
    printf("\n" PROMPT);
    video_cursor_pos(&pos);
    while(getch != NULL)
    {
        char c = getch();
        switch(c)
        {
            case '\b':
                putch(c);
                video_cursor_pos(&pos);
                if(pos.x < (int) strlen(PROMPT))
                {
                    putch(' ');
                    break;
                }
                else
                {
                    command_string[strlen(command_string) - 1] = 0;
                }
                write((int)stdout, " \b", 2);
                break;
            case 0x03: // CTRL + c
                write((int) stdout, "^C", 3);
            case '\n':
                putch('\n');
                if(c == '\n')
                    debug_command(command_string);
                printf(PROMPT);
                command_string[0] = 0;
                break;
            case 0x1b:
                c = getch();
                if(c == 0x5b)
                /* arrow keys here
                    U = A
                    D = B
                    R = C
                    L = D
                */
                c = getch();
                break;
            default:
                if(strlen(command_string) < MAX_COMMAND_LINE_LENGTH)
                {
                    putch(c);
                    strcat(command_string, (char[]){c, 0});
                }
                break;
        }
    }
    panic("\ngetchar", 2);
}

void __attribute__((constructor)) debug_shell_init(void)
{
    // reister built in commands
    command_list[command_list_size++] = (command_t) {"clear", (void*) command_clear,          "Clear the screen"};
    command_list[command_list_size++] = (command_t) {"exit",  (void*) command_exit,           "Exit the shell"};
    command_list[command_list_size++] = (command_t) {"halt",  (void*) command_exit,           "Halt the machine"};
    command_list[command_list_size++] = (command_t) {"echo",  command_echo,                   "Print out some text"};

    // load code into context driver and schedule the entry
    shell_context = malloc(sizeof(ps_context_t));
    mem_entry_t* m = (mem_entry_t*) ((uint32_t) shell_context - sizeof(mem_entry_t));
    m->type = EXE;
    shell_context->name = "SHELL";
    shell_context->code = (uint32_t) debug_shell;
    shell_context->vaddr = (uint32_t) debug_shell;
    shell_context->size = (uint32_t) debug_shell;

    exec_add_schedule(shell_context, (uint32_t) debug_shell);
}
