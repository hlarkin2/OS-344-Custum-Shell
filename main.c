#include "input.h"
#include "builtin.h"
#include "executor.h"
#include "utils.h"
#include "preview.h"

#include <stdio.h>
#include <stdlib.h>


int main() {
    char *input;
    char **args;
    int status = 1;

    print_welcome_banner();

    while (status) {
        printf("\033[0;32medushell> \033[0m");
        input = read_input();

        if (!input) {
            printf("\n");
            break;
        }

        args = parse_input(input);
        status = execute(args);

        free(input);
        free(args);
    }

    return 0;
}
