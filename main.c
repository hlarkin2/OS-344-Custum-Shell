#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

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

// Parse input into arguments
char** parse_input(char *input) {
    char **args = malloc(MAX_ARGS * sizeof(char*));
    if (!args) {
        fprintf(stderr, "Allocation error\n");
        exit(1);
    }

    char *token;
    int position = 0;

    // Tokenize the input string
    token = strtok(input, " \t\r\n\a");
    while (token != NULL) {
        args[position] = token;
        position++;

        if (position >= MAX_ARGS) {
            fprintf(stderr, "Too many arguments\n");
            break;
        }

        token = strtok(NULL, " \t\r\n\a");
    }
    args[position] = NULL; // NULL terminate the array
    return args;
}

// Built-in command: cd
int shell_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "customshell: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("customshell");
        }
    }
    return 1;
}

// Built-in command: exit
int shell_exit(char **args) {
    return 0;
}

// Built-in command: help
int shell_help(char **args) {
    printf("Beginner Friendly Custom Shell\n");
    printf("Type command names and arguments, then press enter.\n");
    printf("Built-in commands:\n");
    printf("  cd <directory>  - Change directory\n");
    printf("  help            - Display this help message\n");
    printf("  exit            - Exit the shell\n");
    return 1;
}

// List of built-in commands
char *builtin_str[] = {
    "cd",
    "exit",
    "help"
};

int (*builtin_func[]) (char **) = {
    &shell_cd,
    &shell_exit,
    &shell_help
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

// Launch a program
int launch_program(char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("customshell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Fork error
        perror("customshell");
    } else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// Preview what a command will do
void preview_command(char **args) {
    if (args[0] == NULL) return;

    printf("\n[PREVIEW] ");

    // Built-in commands
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] != NULL) {
            printf("Will change directory to: %s\n", args[1]);
        } else {
            printf("Will attempt to change directory (no path specified)\n");
        }
    } else if (strcmp(args[0], "exit") == 0) {
        printf("Will exit the shell\n");
    } else if (strcmp(args[0], "help") == 0) {
        printf("Will display help information\n");
    }
    // Common file operations
    else if (strcmp(args[0], "rm") == 0) {
        printf("Will DELETE file(s): ");
        for (int i = 1; args[i] != NULL; i++) {
            printf("%s ", args[i]);
        }
        printf("\n");
    } else if (strcmp(args[0], "mv") == 0) {
        if (args[1] && args[2]) {
            printf("Will MOVE/RENAME '%s' to '%s'\n", args[1], args[2]);
        } else {
            printf("Will attempt to move/rename files\n");
        }
    } else if (strcmp(args[0], "cp") == 0) {
        if (args[1] && args[2]) {
            printf("Will COPY '%s' to '%s'\n", args[1], args[2]);
        } else {
            printf("Will attempt to copy files\n");
        }
    } else if (strcmp(args[0], "mkdir") == 0) {
        printf("Will CREATE directory: ");
        for (int i = 1; args[i] != NULL; i++) {
            printf("%s ", args[i]);
        }
        printf("\n");
    } else if (strcmp(args[0], "touch") == 0) {
        printf("Will CREATE/UPDATE file: ");
        for (int i = 1; args[i] != NULL; i++) {
            printf("%s ", args[i]);
        }
        printf("\n");
    } else if (strcmp(args[0], "cat") == 0) {
        printf("Will DISPLAY contents of: ");
        for (int i = 1; args[i] != NULL; i++) {
            printf("%s ", args[i]);
        }
        printf("\n");
    } else if (strcmp(args[0], "ls") == 0) {
        printf("Will LIST files");
        if (args[1] != NULL) {
            printf(" in: %s", args[1]);
        }
        printf("\n");
    } else if (strcmp(args[0], "chmod") == 0) {
        if (args[1] && args[2]) {
            printf("Will CHANGE PERMISSIONS to '%s' for: %s\n", args[1], args[2]);
        } else {
            printf("Will attempt to change file permissions\n");
        }
    } else if (strcmp(args[0], "chown") == 0) {
        printf("Will CHANGE OWNERSHIP of files\n");
    }
    // General external command
    else {
        printf("Will execute command: %s", args[0]);
        if (args[1] != NULL) {
            printf(" with arguments: ");
            for (int i = 1; args[i] != NULL; i++) {
                printf("%s ", args[i]);
            }
        }
        printf("\n");
    }
}

// Ask user for confirmation
int confirm_execution() {
    char response[10];
    printf("Execute this command? (y/n): ");
    
    if (fgets(response, sizeof(response), stdin) == NULL) {
        return 0;
    }
    
    return (response[0] == 'y' || response[0] == 'Y');
}

// Execute command
int execute(char **args) {
    if (args[0] == NULL) {
        // Empty command
        return 1;
    }

    // Show preview
    preview_command(args);
    
    // Ask for confirmation
    if (!confirm_execution()) {
        printf("Command cancelled.\n");
        return 1;
    }

    // Check for built-in commands
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    // Launch external program
    return launch_program(args);
}

int main() {
    char *input;
    char **args;
    int status = 1;

    // Main loop
    while (status) {
        printf("customshell> ");
        input = read_input();

        if (input == NULL) {
            // Handle EOF (Ctrl+D)
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