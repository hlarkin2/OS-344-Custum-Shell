#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>

#define SHELL_MAX_INPUT 1024
#define MAX_ARGS 64

// Read input from user
char* read_input() {
    char *input = malloc(SHELL_MAX_INPUT);
    if (!input) {
        fprintf(stderr, "Allocation error\n");
        exit(1);
    }

    if (fgets(input, SHELL_MAX_INPUT, stdin) == NULL) {
        free(input);
        return NULL;
    }

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
    args[position] = NULL;
    return args;
}

#include <stdio.h>
#include <unistd.h>  // For usleep()

void print_ascii_typing(const char *str, useconds_t delay_us) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
        fflush(stdout);   
        usleep(delay_us); 
    }
}

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

    print_ascii_typing(ascii_art, 675); 

    printf("Welcome! EduShell is an interactive shell aimed at helping novice Linux users\n");
    printf("become more comfortable working within the Linux terminal.\n\n");
    printf("To begin, type a Linux command to see what it does, or type \"help\" if you\n");
    printf("don't know where to start.\n\n");
}




// Built-in cd
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

// exit
int shell_exit(char **args) { return 0; }

// help
int shell_help(char **args) {
    printf("\nWelcome to the Linux terminal!\n");
    printf("Begin by entering a valid command, such as 'ls' to list files in the current directory,\n");
    printf("or 'cd <path/to/new/directory>' to change directories.\n\n");
    
    printf("Here is a list of possible commands you can enter:\n\n");

    // Custom built-in commands
    printf("Custom commands:\n");
    printf("  help:   prints tips to get you started and shows this help message\n");
    printf("  cd:     changes the current directory to the specified path\n");
    printf("  exit:   exits EduShell\n\n");

    // Standard Linux commands supported in this shell
    printf("Standard Linux commands:\n");
    printf("  ls:     lists files and directories\n");
    printf("  pwd:    prints the current working directory\n");
    printf("  rm:     removes files\n");
    printf("  mv:     moves/renames files\n");
    printf("  cp:     copies files\n");
    printf("  mkdir:  creates new directories\n");
    printf("  touch:  creates or updates a file\n");
    printf("  cat:    displays the contents of a file\n");
    printf("  grep:   searches for patterns in a file\n");
    printf("  chmod:  changes file permissions\n");
    printf("  chown:  changes file ownership\n");
    printf("  ps:     lists current running processes\n");
    printf("  top:    shows an interactive view of running processes\n\n");

    printf("To get started, just type a command and press Enter!\n\n");

    return 1;
}


// Built-in tables
char *builtin_str[] = { "cd", "exit", "help" };
int (*builtin_func[]) (char **) = { &shell_cd, &shell_exit, &shell_help };

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

// Launch a program
int launch_program(char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("customshell");
        }
        exit(EXIT_FAILURE);
    } 
    else if (pid < 0) {
        perror("customshell");
    } 
    else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// Join args
char *join_args(char **args) {
    size_t len = 0;
    for (int i = 0; args[i] != NULL; i++)
        len += strlen(args[i]) + 1;

    char *joined = malloc(len + 1);
    if (!joined) return NULL;
    joined[0] = '\0';

    for (int i = 0; args[i] != NULL; i++) {
        strcat(joined, args[i]);
        if (args[i+1]) strcat(joined, " ");
    }
    return joined;
}

// Check PATH
int command_in_path(const char *cmd) {
    if (!cmd) return 0;
    if (strchr(cmd, '/'))
        return access(cmd, X_OK) == 0;

    char *path_env = getenv("PATH");
    if (!path_env) return 0;

    char *pathdup = strdup(path_env);
    if (!pathdup) return 0;

    char *saveptr = NULL;
    char *dir = strtok_r(pathdup, ":", &saveptr);

    while (dir) {
        char candidate[1024];
        snprintf(candidate, sizeof(candidate), "%s/%s", dir, cmd);
        if (access(candidate, X_OK) == 0) {
            free(pathdup);
            return 1;
        }
        dir = strtok_r(NULL, ":", &saveptr);
    }

    free(pathdup);
    return 0;
}

// Known commands
int is_known_command(const char *cmd) {
    if (!cmd) return 0;

    for (int i = 0; i < num_builtins(); i++)
        if (strcmp(cmd, builtin_str[i]) == 0) return 1;

    const char *known[] = {
        "rm","mv","cp","mkdir","touch","cat","ls","pwd","grep",
        "chmod","chown","ps","top"
    };
    int known_count = sizeof(known)/sizeof(known[0]);

    for (int i = 0; i < known_count; i++)
        if (strcmp(cmd, known[i]) == 0) return 1;

    return 0;
}

int is_valid_command(char *cmd) {
    if (is_known_command(cmd)) return 1;
    if (command_in_path(cmd)) return 1;
    return 0;
}


int validate_usage(char **args) {
    if (!args[0]) return 0;

    // cd: require a path
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            printf("Incorrect use of cd.\n");
            printf("Syntax: cd <directory>\n");
            printf("Example: cd Documents\n");
            return 0;
        }
        return 1;
    }

    // rm
    if (strcmp(args[0], "rm") == 0) {
        if (!args[1]) {
            printf("Incorrect use of rm.\n");
            printf("Syntax: rm <file1> [file2 ...]\n");
            printf("Example: rm text.txt\n");
            return 0;
        }
        return 1;
    }

    // mv
    if (strcmp(args[0], "mv") == 0) {
        if (!args[1] || !args[2]) {
            printf("Incorrect use of mv.\n");
            printf("Syntax: mv <source> <destination>\n");
            printf("Example: mv old.txt new.txt\n");
            return 0;
        }
        return 1;
    }

    // cp
    if (strcmp(args[0], "cp") == 0) {
        if (!args[1] || !args[2]) {
            printf("Incorrect use of cp.\n");
            printf("Syntax: cp <src> <dest>\n");
            printf("Example: cp a.txt b.txt\n");
            return 0;
        }
        return 1;
    }

    // mkdir
    if (strcmp(args[0], "mkdir") == 0) {
        if (!args[1]) {
            printf("Incorrect use of mkdir.\n");
            printf("Syntax: mkdir <dir>\n");
            printf("Example: mkdir newFolder\n");
            return 0;
        }
        return 1;
    }

    // touch
    if (strcmp(args[0], "touch") == 0) {
        if (!args[1]) {
            printf("Incorrect use of touch.\n");
            printf("Syntax: touch <file>\n");
            printf("Example: touch notes.txt\n");
            return 0;
        }
        return 1;
    }

    // cat
    if (strcmp(args[0], "cat") == 0) {
        if (!args[1]) {
            printf("Incorrect use of cat.\n");
            printf("Syntax: cat <file>\n");
            printf("Example: cat text.txt\n");
            return 0;
        }
        return 1;
    }

    // chmod
    if (strcmp(args[0], "chmod") == 0) {
        if (!args[1] || !args[2]) {
            printf("Incorrect use of chmod.\n");
            printf("Syntax: chmod <mode> <file>\n");
            printf("Example: chmod 644 file.txt\n");
            return 0;
        }
        return 1;
    }

    // chown
    if (strcmp(args[0], "chown") == 0) {
        if (!args[1] || !args[2]) {
            printf("Incorrect use of chown.\n");
            printf("Syntax: chown <owner> <file>\n");
            printf("Example: chown user file.txt\n");
            return 0;
        }
        return 1;
    }

    // grep
    if (strcmp(args[0], "grep") == 0) {
        if (!args[1]) {
            printf("Incorrect use of grep.\n");
            printf("Syntax: grep <pattern> <file(s)>\n");
            printf("Example: grep \"hello\" file.txt\n");
            return 0;
        }
        if (strcmp(args[1], "*") == 0 && !args[2]) {
            printf("Incorrect use of grep.\n");
            printf("Syntax: grep <pattern> <file>\n");
            printf("Example: grep \"foo\" file.txt\n");
            return 0;
        }
        return 1;
    }

    return 1;
}


void resolve_cd_target(const char *inputPath, char *resolvedOut) {
    char temp[PATH_MAX];

    if (strcmp(inputPath, "..") == 0) {
        if (realpath("..", temp) != NULL) {
            strcpy(resolvedOut, temp);
            return;
        }
    }

    if (realpath(inputPath, temp) != NULL) {
        strcpy(resolvedOut, temp);
        return;
    }

    // fallback if realpath fails
    snprintf(resolvedOut, PATH_MAX, "%s/%s", getenv("PWD"), inputPath);
}

// Preview
void preview_command(char **args) {
    if (!args[0]) return;

    char *joined = join_args(args);
    if (!joined) joined = strdup(args[0]);

    printf("\n[PREVIEW] ");

    if (strcmp(args[0], "cd") == 0) {
        if (args[1]) {
            char resolved[PATH_MAX];
            resolve_cd_target(args[1], resolved);
            printf("%s will change the current directory to '%s'.\n", joined, resolved);
        } else {
            printf("%s will attempt to change the directory (no path given).\n", joined);
        }
    }

    else if (strcmp(args[0], "exit") == 0)
        printf("%s will exit the shell.\n", joined);

    else if (strcmp(args[0], "help") == 0)
        printf("%s will display help.\n", joined);

    else if (strcmp(args[0], "rm") == 0) {
        printf("%s will REMOVE: ", joined);
        for (int i = 1; args[i]; i++) {
            printf("%s", args[i]);
            if (args[i+1]) printf(" ");
        }
        printf("\n");
    }

    else if (strcmp(args[0], "mv") == 0)
        printf("%s will MOVE '%s' to '%s'.\n", joined, args[1], args[2]);

    else if (strcmp(args[0], "cp") == 0)
        printf("%s will COPY '%s' to '%s'.\n", joined, args[1], args[2]);

    else if (strcmp(args[0], "mkdir") == 0) {
        printf("%s will CREATE directories: ", joined);
        for (int i = 1; args[i]; i++) {
            printf("%s", args[i]);
            if (args[i+1]) printf(" ");
        }
        printf("\n");
    }

    else if (strcmp(args[0], "touch") == 0) {
        printf("%s will CREATE/UPDATE: ", joined);
        for (int i = 1; args[i]; i++) {
            printf("%s", args[i]);
            if (args[i+1]) printf(" ");
        }
        printf("\n");
    }

    else if (strcmp(args[0], "cat") == 0) {
        printf("%s will DISPLAY: ", joined);
        for (int i = 1; args[i]; i++) {
            printf("%s", args[i]);
            if (args[i+1]) printf(" ");
        }
        printf("\n");
    }

    else if (strcmp(args[0], "ls") == 0) {
        if (args[1])
            printf("%s will LIST files in: %s\n", joined, args[1]);
        else
            printf("%s will LIST files in the current directory.\n", joined);
    }

    else if (strcmp(args[0], "pwd") == 0)
        printf("%s will PRINT the current directory.\n", joined);

    else if (strcmp(args[0], "grep") == 0) {
        printf("%s will SEARCH for pattern '%s' in: ", joined, args[1]);
        for (int i = 2; args[i]; i++) {
            printf("%s", args[i]);
            if (args[i+1]) printf(" ");
        }
        printf("\n");
    }

    else if (strcmp(args[0], "chmod") == 0)
        printf("%s will CHANGE permissions to '%s' for '%s'.\n", joined, args[1], args[2]);

    else if (strcmp(args[0], "chown") == 0)
        printf("%s will CHANGE owner to '%s' for '%s'.\n", joined, args[1], args[2]);

    else {
        printf("%s will execute an external program.\n", joined);
    }

    free(joined);
}

int confirm_execution() {
    char response[10];
    printf("Execute this command? (y/n): ");
    if (!fgets(response, sizeof(response), stdin)) return 0;
    printf("\n"); 
    return (response[0] == 'y' || response[0] == 'Y');
}

int execute(char **args) {
    if (!args[0]) return 1;

    if (!is_valid_command(args[0])) {
        printf("Invalid command: %s\n", args[0]);
        return 1;
    }

    // strict validation
    if (!validate_usage(args))
        return 1;

    preview_command(args);

    if (!confirm_execution()) {
        printf("Command cancelled.\n");
        return 1;
    }

    for (int i = 0; i < num_builtins(); i++)
        if (strcmp(args[0], builtin_str[i]) == 0)
            return (*builtin_func[i])(args);

    return launch_program(args);
}

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
