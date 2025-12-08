#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <ctype.h>
#include "utils.h"



char last_real_command[64] = "";


CommandHistory history[HISTORY_SIZE];
int history_count = 0;


// ---------------------------------------------------------------------------
// Check if command is supported (builtin or valid Linux command)
// ---------------------------------------------------------------------------
int is_valid_command(char *cmd) {
    if (!cmd) return 0;

    // Built-ins
    const char *builtins[] = { "cd", "exit", "help", "next", "history", "wlfc", "source" };
    int num_builtins = sizeof(builtins) / sizeof(char*);

    for (int i = 0; i < num_builtins; i++)
        if (strcmp(cmd, builtins[i]) == 0) return 1;

    // Standard commands
    const char *valid_cmds[] = {
        "ls","pwd","rm","mv","cp","mkdir","touch","cat",
        "grep","chmod","chown","ps","top"
    };
    int num_std = sizeof(valid_cmds) / sizeof(char*);

    for (int i = 0; i < num_std; i++)
        if (strcmp(cmd, valid_cmds[i]) == 0) return 1;

    // Check if in PATH
    return command_in_path(cmd);
}

void set_last_real_command(const char *cmd) {
    if (cmd && strcmp(cmd, "next") != 0 && strlen(cmd) < 64) {
        strcpy(last_real_command, cmd);
    }
}

const char* get_last_real_command() {
    return last_real_command;
}

void add_to_history(const char *cmd) {
    if (!cmd || strlen(cmd) == 0) return;

    // Shift history if full
    if (history_count == HISTORY_SIZE) {
        for (int i = 1; i < HISTORY_SIZE; i++) {
            history[i-1] = history[i];
        }
        history_count--;
    }

    // Add new command
    strncpy(history[history_count].command, cmd, sizeof(history[history_count].command)-1);
    history[history_count].command[sizeof(history[history_count].command)-1] = '\0';

    // Set a short description
    if (strcmp(cmd, "ls") == 0)
        strcpy(history[history_count].description, "listed the contents of the current directory");
    else if (strcmp(cmd, "pwd") == 0)
        strcpy(history[history_count].description, "printed the current working directory");
    else if (strcmp(cmd, "cd") == 0)
        strcpy(history[history_count].description, "changed the current directory");
    else if (strcmp(cmd, "mkdir") == 0)
        strcpy(history[history_count].description, "created a new directory");
    else if (strcmp(cmd, "touch") == 0)
        strcpy(history[history_count].description, "created a new file or updated its timestamp");
    else if (strcmp(cmd, "rm") == 0)
        strcpy(history[history_count].description, "removed a file or directory");
    else if (strcmp(cmd, "mv") == 0)
        strcpy(history[history_count].description, "moved or renamed a file or directory");
    else if (strcmp(cmd, "cp") == 0)
        strcpy(history[history_count].description, "copied a file or directory");
    else if (strcmp(cmd, "cat") == 0)
        strcpy(history[history_count].description, "printed the contents of a file");
    else if (strcmp(cmd, "grep") == 0)
        strcpy(history[history_count].description, "searched for a pattern in a file");
    else if (strcmp(cmd, "chmod") == 0)
        strcpy(history[history_count].description, "changed file permissions");
    else if (strcmp(cmd, "chown") == 0)
        strcpy(history[history_count].description, "changed file ownership");
    else if (strcmp(cmd, "next") == 0)
        strcpy(history[history_count].description, "suggested the next useful commands");
    else if (strcmp(cmd, "help") == 0)
        strcpy(history[history_count].description, "displayed help information");
    else if (strcmp(cmd, "wlfc") == 0)
        strcpy(history[history_count].description, "who loves fried chicken?");
    else if (strcmp(cmd, "history") == 0)
        strcpy(history[history_count].description, "displayed the last 10 commands typed with descriptions");
    else
        strcpy(history[history_count].description, "executed an external command");

    history_count++;
}




// ---------------------------------------------------------------------------
// Validate proper syntax for supported commands
// ---------------------------------------------------------------------------
int validate_usage(char **args) {
    if (!args || !args[0]) return 0;

    // cd
    if (strcmp(args[0],"cd")==0) {
        if (!args[1]) {
            printf("Incorrect use of cd.\n");
            printf("Syntax: cd <directory>\n");
            printf("Example: cd Documents\n");
            return 0;
        }
        return 1;
    }

    // rm
    if (strcmp(args[0],"rm")==0) {
        if (!args[1]) {
            printf("Incorrect use of rm.\n");
            printf("Syntax: rm <file1> [file2 ...]\n");
            printf("Example: rm notes.txt\n");
            return 0;
        }
        return 1;
    }

    // mv
    if (strcmp(args[0],"mv")==0) {
        if (!args[1] || !args[2]) {
            printf("Incorrect use of mv.\n");
            printf("Syntax: mv <source> <destination>\n");
            printf("Example: mv old.txt new.txt\n");
            return 0;
        }
        return 1;
    }

    // cp
    if (strcmp(args[0],"cp")==0) {
        if (!args[1] || !args[2]) {
            printf("Incorrect use of cp.\n");
            printf("Syntax: cp <src> <dest>\n");
            printf("Example: cp a.txt b.txt\n");
            return 0;
        }
        return 1;
    }

    // mkdir
    if (strcmp(args[0],"mkdir")==0) {
        if (!args[1]) {
            printf("Incorrect use of mkdir.\n");
            printf("Syntax: mkdir <dir>\n");
            printf("Example: mkdir newFolder\n");
            return 0;
        }
        return 1;
    }

    // touch
    if (strcmp(args[0],"touch")==0) {
        if (!args[1]) {
            printf("Incorrect use of touch.\n");
            printf("Syntax: touch <file>\n");
            printf("Example: touch notes.txt\n");
            return 0;
        }
        return 1;
    }

    // cat
    if (strcmp(args[0],"cat")==0) {
        if (!args[1]) {
            printf("Incorrect use of cat.\n");
            printf("Syntax: cat <file>\n");
            printf("Example: cat text.txt\n");
            return 0;
        }
        return 1;
    }

    // chmod
    if (strcmp(args[0],"chmod")==0) {
        if (!args[1] || !args[2]) {
            printf("Incorrect use of chmod.\n");
            printf("Syntax: chmod <mode> <file>\n");
            printf("Example: chmod 644 file.txt\n");
            return 0;
        }
        return 1;
    }

    // chown
    if (strcmp(args[0],"chown")==0) {
        if (!args[1] || !args[2]) {
            printf("Incorrect use of chown.\n");
            printf("Syntax: chown <owner> <file>\n");
            printf("Example: chown user file.txt\n");
            return 0;
        }
        return 1;
    }

    // grep
    if (strcmp(args[0],"grep")==0) {
        if (!args[1]) {
            printf("Incorrect use of grep.\n");
            printf("Syntax: grep <pattern> <file>\n");
            printf("Example: grep \"hello\" file.txt\n");
            return 0;
        }
        return 1;
    }

    return 1;
}

// ---------------------------------------------------------------------------
// Join arguments back into a single string
// ---------------------------------------------------------------------------
char *join_args(char **args) {
    if (!args || !args[0]) return NULL;

    char buffer[2048] = {0};
    for (int i=0; args[i]; i++) {
        strcat(buffer, args[i]);
        if (args[i+1]) strcat(buffer, " ");
    }
    return strdup(buffer);
}

// ---------------------------------------------------------------------------
// Check command exists in $PATH
// ---------------------------------------------------------------------------
int command_in_path(const char *cmd) {
    if (!cmd) return 0;

    char *path = getenv("PATH");
    if (!path) return 0;

    char temp[PATH_MAX];
    char *copy = strdup(path);
    char *dir = strtok(copy, ":");

    while (dir) {
        snprintf(temp, sizeof(temp), "%s/%s", dir, cmd);
        if (access(temp, X_OK) == 0) {
            free(copy);
            return 1;
        }
        dir = strtok(NULL, ":");
    }
    free(copy);
    return 0;
}

// ---------------------------------------------------------------------------
// Ask user "Execute this command? (y/n)"
// ---------------------------------------------------------------------------
int confirm_execution() {
    printf("Execute this command? (y/n): ");
    char ans[8];
    fgets(ans, sizeof(ans), stdin);

    if (ans[0] == 'y' || ans[0] == 'Y') {
        printf("\n"); // <-- ADDED NEWLINE YOU ASKED FOR
        return 1;
    }
    return 0;
}
