#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAZ_ARGS 64

// Read input from user
char* read_input() {
    char *input = malloc(MAX_INPUT);
    if (!input) {
        fprintf(stderr, "Allocation error\n");
        exit(1);
    }

    if (fgets(input, MAX_INPUT, stdin) == NULL) {
        free(input);
        return NULL;
    }

    // remove newline character
    input[strcspn(input, "\n")] = 0;
    return input;
}

// parse inputs
char** parse_input(char *input) {
    char **args = malloc(MAX_ARGS * sizeof(char*));
    if (!args) {
        fprintf(stderr, "Allocation error\n");
        exit(1);
    }
}

int main() {
    char *input;
    char **args;
    int status = 1;

    // main loop
    while (status) {
        printf("customshell> ");
    }

    return 0;
}