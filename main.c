#include "input.h"
#include "builtin.h"
#include "executor.h"
#include "utils.h"
#include "preview.h"

#include <stdio.h>
#include <stdlib.h>


//scipting function
void run_script(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Error opening script");
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0; // remove newline

        // Show the user what is happening
        printf("[SCRIPT] %s\n", line);

        char *copy = strdup(line);
        char **args = parse_input(copy);

        execute_command(args);

        free(copy);
        free(args);
    }

int main(int argc, char *argv[]) {
    // If a script file was provided, run it and exit
    if (argc == 2) {
        run_script(argv[0]);
        return 0;
    }

    // Otherwise interactive shell
    char *input;
    char **args;
    int status = 1;

    while (status) {
        printf("customshell> ");
        input = read_input();

        // If user types: script filename
        if (strncmp(input, "run ", 4) == 0) {
            run_script(input + 4);
            free(input);
            continue;
        }

        args = parse_input(input);
        status = execute_command(args);

        free(input);
        free(args);
    }

    return 0;
}

