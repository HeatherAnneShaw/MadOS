////////////////////////////////////////////////////////////////////////////////
// THE SCOTCH-WARE LICENSE (Revision 0):
// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a shot of scotch in return
////////////////////////////////////////////////////////////////////////////////


#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdlib.h>

#define MAX_COMMAND_LENGTH 40
#define MAX_COMMAND_LINE_LENGTH MAX_COMMAND_LENGTH * 2
#define MAX_COMMAND_LINE_VECTOR_LENGTH MAX_COMMAND_LINE_LENGTH

typedef void (*command_fun_ptr) (int, char**);

typedef struct command_t {
    char* command;
    command_fun_ptr function;
    char* description;
} command_t;

extern bool register_shell_command(char* command, command_fun_ptr function, char* description);

#endif
