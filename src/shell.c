
#include <video.h>
#include <stdio.h>
#include <string.h>

typedef void (*command_fun_ptr) (char**);

typedef struct command_t {
    char command[256];
    command_fun_ptr function;
} command_t;

void command_clear(char** command)
{
    command+=1;
    video_clear();
}
extern void halt(void);
void command_exit(char** command)
{
    command+=1;
    halt();
}

#define command_list_size 3
command_t command_list[command_list_size];
void __attribute__((constructor)) debug_shell_init(void)
{
    command_list[0] = (command_t) {"clear", command_clear};
    command_list[1] = (command_t) {"exit", command_exit};
    command_list[2] = (command_t) {"halt", command_exit};
}

void debug_command(char* command_string)
{
    char* commandv[256];
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
        for(int i = 0;i < command_list_size;i++)
        {
            if(strcmp(command_list[i].command, commandv[0]) == 0)
            {
                command_list[i].function(commandv);
                return;
            }
        }
        printf("%s: command not found\n", commandv[0]);
}

void __attribute__((destructor)) debug_shell(void)
{
    cursor_pos_t pos;
    char command_string[512] = "";
    printf("\nMadOS> ");
    while(getch != NULL)
    {
        char c = getch();
        putch(c);
        switch(c)
        {
            case '\b':
                video_cursor_pos(&pos);
                if(pos.x < 7)
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
            case '\n':
                debug_command(command_string);
                printf("MadOS> ");
                command_string[0] = 0;
                break;
            default:
                strcat(command_string, (char[]){c, 0});
        }
    }
    panic("\ngetchar", 2);
}
