#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

#define SHELL_MAX_INPUT 1024
#define MAX_ARGS 64

char* read_input() {
    char *input = malloc(SHELL_MAX_INPUT);
    if (!input) { fprintf(stderr, "Allocation error\n"); exit(1); }

    if (fgets(input, SHELL_MAX_INPUT, stdin) == NULL) {
        free(input);
        return NULL;
    }

    input[strcspn(input, "\n")] = 0;
    return input;
}

char** parse_input(char *input) {
    char **args = malloc(MAX_ARGS * sizeof(char*));
    if (!args) { fprintf(stderr, "Allocation error\n"); exit(1); }

    char *token;
    int position = 0;
    token = strtok(input, " \t\r\n\a");

    while (token != NULL) {
        args[position++] = token;
        if (position >= MAX_ARGS) { fprintf(stderr, "Too many arguments\n"); break; }
        token = strtok(NULL, " \t\r\n\a");
    }

    args[position] = NULL;
    return args;
}
