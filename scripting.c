#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "scripting.h"

#define MAX_ARGS 64

void explain_command(char **args) {
    if (args[0] == NULL) return;

    if (strcmp(args[0], "ls") == 0) {
        printf("[EXPLAIN] 'ls' lists the contents of a directory. Options like '-l' show detailed info.\n");
    } else if (strcmp(args[0], "cp") == 0) {
        printf("[EXPLAIN] 'cp' copies files from source to destination.\n");
    } else if (strcmp(args[0], "cd") == 0) {
        printf("[EXPLAIN] 'cd' changes the current working directory.\n");
    } else {
        printf("[EXPLAIN] Command '%s' will be executed.\n", args[0]);
    }
}
