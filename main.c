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