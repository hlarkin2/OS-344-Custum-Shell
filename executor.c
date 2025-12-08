#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>       // must be above exit(), EXIT_FAILURE
#include "builtin.h"
#include "utils.h"
#include "preview.h"
#include "executor.h"

// -------------------------------------------
// Strip surrounding single or double quotes from arguments
// -------------------------------------------
void strip_quotes(char **args) {
    for (int i = 0; args[i]; i++) {
        size_t len = strlen(args[i]);
        if ((args[i][0] == '"' && args[i][len-1] == '"') ||
            (args[i][0] == '\'' && args[i][len-1] == '\'')) {
            args[i][len-1] = '\0';  // remove trailing quote
            args[i]++;               // move pointer past leading quote
        }
    }
}



int execute(char **args) {

    if (!args[0])
        return 1;

    // Educational invalid command checker
    if (!is_valid_command(args[0])) {
        printf("Invalid command: %s\n", args[0]);
        return 1;
    }

    // Usage checker (syntax validation)
    if (!validate_usage(args))
        return 1;

    // Update last valid command (ignore 'next')
    if (args[0] && strcmp(args[0], "next") != 0) {
    set_last_real_command(args[0]);
    }

    add_to_history(args[0]);


    // Show preview of what the command will do
    preview_command(args);

    // Ask user to confirm running the command
    if (!confirm_execution()) {
        printf("Command cancelled.\n");
        return 1;
    }

    // Builtin command handler
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    strip_quotes(args);

    // External commands (fork + execvp)
    pid_t pid = fork();
    int status;

    if (pid == 0) {
        // Child executes command
        if (execvp(args[0], args) == -1)
            perror("customshell");
        exit(EXIT_FAILURE);
    } 
    else if (pid < 0) {
        perror("customshell");
    } 
    else {
        // Parent waits for child
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}
