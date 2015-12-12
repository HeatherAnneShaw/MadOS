
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

void command_clear(void)
{
    video_clear();
    putch('\n');
}
extern void halt(void);
void command_exit(void)
{
    printf("< shell terminated >\n");
    halt();
}

/////////////////////////////////////////////////////////////////////////////////

int grid_check_neighbors(bool* grid, int width, int height, int x, int y)
{
#define SIZE (width * height)
#define STATE(x, y, limit_comp) (int) (((x) * width + (y)) limit_comp ? grid[(x) * width + (y)] : false)
    return STATE(x-1, y-1,>=0) + STATE(x-1,y,>=0) + STATE(x-1,y+1,>=0) + STATE(x,y-1,>=0) +
        STATE(x,y+1,<SIZE) + STATE(x+1,y-1,<SIZE) + STATE(x+1,y,<SIZE) + STATE(x+1,y+1,<SIZE);
}


void grid_step(bool* grid, int width, int height)
{
#define SIZE (width * height)
    bool t[SIZE];
    memcpy(t, grid, SIZE*sizeof(bool));
    for(int i = 0, n = 0;i < SIZE;i++)
    {
        n = grid_check_neighbors(grid, width, height, i / width, i % width);
        if(!grid[i] && n == 3)
                t[i] = true;
        else if(n < 2 || n > 3)
                t[i] = false;
    }
    memcpy(grid, t, SIZE*sizeof(bool));
}

void command_bored(void)
{
    #define SIZE (width * height)
    const int width = 25,
        height = 25;
    int frames = 560;
    
    bool grid[] = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,
        0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,1,0,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    
    video_clear();
    for(int i = 0;i < frames;i++)
    {
        video_update_cursor(0,0);
        for(int c = 0;c < SIZE;c++)
        {
            printf("  ");
            if(grid[c])
            video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_RED));
            else if(c % 2)
                video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_BLACK));
            else
                video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_WHITE));
            if((c + 1) % width == 0)
            {
                video_setcolor(MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_BLACK));
                puts("");
            }
        }
        grid_step(grid, width, height);
        //usleep(1000*100);
    }
}

/////////////////////////////////////////////////////////////////////////////////

#define command_list_size 4
command_t command_list[command_list_size];
void __attribute__((constructor)) debug_shell_init(void)
{
    command_list[0] = (command_t) {"clear", (void*) command_clear};
    command_list[1] = (command_t) {"exit", (void*) command_exit};
    command_list[2] = (command_t) {"halt", (void*) command_exit};
    command_list[2] = (command_t) {"B==D", (void*) command_bored};
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
