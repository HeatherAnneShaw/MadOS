
#include <video.h>
#include <stdio.h>
#include <string.h>
#include <madmath.h>

#define MAX_COMMAND_LENGTH 256
#define MAX_COMMAND_LINE_LENGTH 512
#define MAX_COMMAND_LINE_VECTOR_LENGTH MAX_COMMAND_LINE_LENGTH

typedef void (*command_fun_ptr) (char**);

typedef struct command_t {
    char command[MAX_COMMAND_LENGTH];
    command_fun_ptr function;
} command_t;

void command_clear(char** command)
{
    command+=1;
    video_clear();
    putch('\n');
}
extern void halt(void);
void command_exit(char** command)
{
    command+=1;
    printf("< shell terminated >\n");
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
    char* commandv[MAX_COMMAND_LINE_VECTOR_LENGTH];
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

#if defined(__arm__)
#define PROMPT "\033[092m(\033[037mMadOS\033[092m)\033[0m "
#else
#define PROMPT "(MadOS) "
#endif
void __attribute__((destructor)) debug_shell(void)
{
    cursor_pos_t pos;
    char command_string[MAX_COMMAND_LINE_LENGTH] = "";
    printf("\n" PROMPT);
    
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
                putch(c);
                strcat(command_string, (char[]){c, 0});
                break;
        }
    }
    panic("\ngetchar", 2);
}
