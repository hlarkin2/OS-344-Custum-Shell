#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "preview.h"

/* ============================================================
   TYPEWRITER-STYLE ASCII OUTPUT
   ============================================================ */
void print_ascii_typing(const char *str, useconds_t delay_us) {
    for (size_t i = 0; str[i]; i++) {
        putchar(str[i]);
        fflush(stdout);
        usleep(delay_us);
    }
}

/* ============================================================
   HELPERS FOR PREVIEWING CD COMMAND
   ============================================================ */
void resolve_cd_target(const char *inputPath, char *resolvedOut) {
    char temp[PATH_MAX];

    if (strcmp(inputPath, "..") == 0 && realpath("..", temp) != NULL) {
        strcpy(resolvedOut, temp);
        return;
    }

    if (realpath(inputPath, temp) != NULL) {
        strcpy(resolvedOut, temp);
        return;
    }

    snprintf(resolvedOut, PATH_MAX, "%s/%s", getenv("PWD"), inputPath);
}

/* ============================================================
   PREVIEW COMMAND (USED BEFORE EXECUTION)
   ============================================================ */
void preview_command(char **args) {
    if (!args[0])
        return;

    char *joined = join_args(args);
    if (!joined)
        joined = strdup(args[0]);

    printf("\n[PREVIEW] ");

    if (strcmp(args[0], "cd") == 0) {
        char resolved[PATH_MAX];
        if (args[1]) {
            resolve_cd_target(args[1], resolved);
            printf("%s will change the current directory to '%s'.\n", joined, resolved);
        } else {
            printf("%s is missing a required argument: directory path.\n", joined);
        }

    } else if (strcmp(args[0], "exit") == 0) {
        printf("%s will exit the shell.\n", joined);

    } else if (strcmp(args[0], "help") == 0) {
        printf("%s will display helpful information about supported commands.\n", joined);

    } else if (strcmp(args[0], "next") == 0) {
        printf("%s will suggest the next useful command based on your activity.\n", joined);

    } else if (strcmp(args[0], "history") == 0) {
        printf("%s will show the last %d commands you executed along with a short description.\n", joined, HISTORY_SIZE);

    } else if (strcmp(args[0], "wlfc") == 0) {
        printf("%s who loves fried chicken?\n", joined);

    } else if (strcmp(args[0], "ls") == 0) {
        printf("%s will list the files and directories in the current working directory.\n", joined);

    } else if (strcmp(args[0], "pwd") == 0) {
        printf("%s will print the current working directory path.\n", joined);

    } else if (strcmp(args[0], "mkdir") == 0) {
        printf("%s will create a new directory. Usage: mkdir <directory_name>\n", joined);

    } else if (strcmp(args[0], "touch") == 0) {
        printf("%s will create a new empty file or update the timestamp of an existing file. Usage: touch <file_name>\n", joined);

    } else if (strcmp(args[0], "rm") == 0) {
        printf("%s will remove a file or directory. Usage: rm <file_name>\n", joined);

    } else if (strcmp(args[0], "mv") == 0) {
        printf("%s will move or rename a file or directory. Usage: mv <source> <destination>\n", joined);

    } else if (strcmp(args[0], "cp") == 0) {
        printf("%s will copy a file or directory. Usage: cp <source> <destination>\n", joined);

    } else if (strcmp(args[0], "cat") == 0) {
        printf("%s will display the contents of a file. Usage: cat <file_name>\n", joined);

    } else if (strcmp(args[0], "grep") == 0) {
        printf("%s will search for a pattern in a file. Usage: grep <pattern> <file>\n", joined);

    } else if (strcmp(args[0], "chmod") == 0) {
        printf("%s will change file permissions. Usage: chmod <mode> <file_name>\n", joined);

    } else if (strcmp(args[0], "chown") == 0) {
        printf("%s will change the ownership of a file. Usage: chown <owner> <file_name>\n", joined);

    } else {
        printf("%s will execute an external program or Linux command.\n", joined);
    }

    free(joined);
}

/* ============================================================
   WELCOME BANNER
   ============================================================ */
void print_welcome_banner() {
    const char *ascii_art =
"  _____                                                                                                  _____ \n"
" ( ___ )                                                                                                ( ___ )\n"
"  |   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|   | \n"
"  |   |  /$$$$$$$$       /$$            /$$$$$$  /$$                 /$$ /$$         /$$        /$$$$$$  |   | \n"
"  |   | | $$_____/      | $$           /$$__  $$| $$                | $$| $$       /$$$$       /$$$_  $$ |   | \n"
"  |   | | $$        /$$$$$$$ /$$   /$$| $$  \\__/| $$$$$$$   /$$$$$$ | $$| $$      |_  $$      | $$$$\\ $$ |   | \n"
"  |   | | $$$$$    /$$__  $$| $$  | $$|  $$$$$$ | $$__  $$ /$$__  $$| $$| $$        | $$      | $$ $$ $$ |   | \n"
"  |   | | $$__/   | $$  | $$| $$  | $$ \\____  $$| $$  \\ $$| $$$$$$$$| $$| $$        | $$      | $$\\ $$$$ |   | \n"
"  |   | | $$      | $$  | $$| $$  | $$ /$$  \\ $$| $$  | $$| $$_____/| $$| $$        | $$      | $$ \\ $$$ |   | \n"
"  |   | | $$$$$$$$|  $$$$$$$|  $$$$$$/|  $$$$$$/| $$  | $$|  $$$$$$$| $$| $$       /$$$$$$ /$$|  $$$$$$/ |   | \n"
"  |   | |________/ \\_______/ \\______/  \\______/ |__/  |__/ \\_______/|__/|__/      |______/|__/ \\______/  |   | \n"
"  |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___| \n"
" (_____)                                                                                                (_____) \n\n";

    print_ascii_typing(ascii_art, 650);

    printf("Welcome! EduShell is an interactive shell aimed at helping novice Linux users\n");
    printf("become more comfortable working within the Linux terminal.\n");
    printf("To begin, type a Linux command to see what it does, or type \"help\" if you\n");
    printf("don't know where to start.\n\n");
}
