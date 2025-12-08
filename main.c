#define _POSIX_C_SOURCE 200809L

#include "input.h"
#include "builtin.h"
#include "executor.h"
#include "utils.h"
#include "preview.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Scripting function
void run_script(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Error opening script");
        return;
    }

    // Enable script mode (disables interactive prompts)
    script_mode = 1;

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0; // remove newline

        // Skip empty lines and comments
        if (strlen(line) == 0 || line[0] == '#') {
            continue;
        }

        // Show the user what is happening
        printf("[SCRIPT] %s\n", line);

        char *copy = strdup(line);
        char **args = parse_input(copy);

        execute(args);

        free(copy);
        free(args);
    }

    fclose(f);
    script_mode = 0;  // Disable script mode
}

int main(int argc, char *argv[]) {
    // If a script file was provided, run it and exit
    if (argc == 2) {
        run_script(argv[1]);
        return 0;
    }

    // Otherwise interactive shell
    print_welcome_banner();
    
    char *input;
    char **args;
    int status = 1;

    while (status) {
        printf("customshell> ");
        input = read_input();

        if (!input) {
            break;  // EOF or error
        }

        // If user types: run filename
        if (strncmp(input, "run ", 4) == 0) {
            run_script(input + 4);
            free(input);
            continue;
        }

        args = parse_input(input);
        status = execute(args);

        free(input);
        free(args);
    }

    return 0;
}