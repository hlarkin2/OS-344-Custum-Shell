// executor.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "builtin.h"
#include "utils.h"
#include "preview.h"
#include "executor.h"



// -------------------------------------------
// Remove surrounding quotes from a string
// -------------------------------------------
void strip_quotes(char *str) {
    size_t len = strlen(str);
    if (len >= 2 && ((str[0] == '"' && str[len-1] == '"') ||
                     (str[0] == '\'' && str[len-1] == '\''))) {
        str[len-1] = '\0';
        memmove(str, str+1, len-1);
    }
}

// -------------------------------------------
// Execute single command with preview & confirmation
// -------------------------------------------
void execute_simple(char *cmdline) {
    char *cmd = strdup(cmdline);
    char *in_file = NULL;
    char *out_file = NULL;
    int append = 0;

    // Input redirection
    char *in_pos = strchr(cmd, '<');
    if (in_pos) {
        *in_pos = '\0';
        in_pos++;
        while (*in_pos == ' ') in_pos++;
        in_file = strtok(in_pos, " \t\n");
        if (in_file) strip_quotes(in_file);
    }

    // Output redirection
    char *out_pos = strstr(cmd, ">>");
    if (out_pos) {
        *out_pos = '\0';
        out_pos += 2;
        while (*out_pos == ' ') out_pos++;
        out_file = strtok(out_pos, " \t\n");
        append = 1;
        if (out_file) strip_quotes(out_file);
    } else {
        out_pos = strchr(cmd, '>');
        if (out_pos) {
            *out_pos = '\0';
            out_pos++;
            while (*out_pos == ' ') out_pos++;
            out_file = strtok(out_pos, " \t\n");
            append = 0;
            if (out_file) strip_quotes(out_file);
        }
    }

    while (*cmd == ' ' || *cmd == '\t') cmd++;

    // Split into argv
    char *argv[64];
    int argc = 0;
    char *token = strtok(cmd, " \t\n");
    while (token && argc < 63) {
        strip_quotes(token);
        argv[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    argv[argc] = NULL;

    if (!argv[0]) { free(cmd); return; }

    // Handle exit
    if (strcmp(argv[0], "exit") == 0) {
        free(cmd);
        exit(0);
    }

    // Preview
    preview_command(argv);
    if (!confirm_execution()) {
        printf("Command cancelled.\n");
        free(cmd);
        return;
    }

    // Built-ins
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(argv[0], builtin_str[i]) == 0) {
            (*builtin_func[i])(argv);
            free(cmd);
            return;
        }
    }

    // External command
    pid_t pid = fork();
    if (pid == 0) {
        if (in_file) {
            int fd = open(in_file, O_RDONLY);
            if (fd < 0) { perror("open input"); exit(EXIT_FAILURE); }
            dup2(fd, 0); close(fd);
        }
        if (out_file) {
            int fd = append ? open(out_file, O_WRONLY|O_CREAT|O_APPEND, 0644)
                            : open(out_file, O_WRONLY|O_CREAT|O_TRUNC, 0644);
            if (fd < 0) { perror("open output"); exit(EXIT_FAILURE); }
            dup2(fd, 1); close(fd);
        }
        execvp(argv[0], argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        int status;
        waitpid(pid, &status, 0);
    }

    free(cmd);
}

// -------------------------------------------
// Execute single command without preview (used for pipelines)
// -------------------------------------------
void execute_simple_no_preview(char *cmdline) {
    char *cmd = strdup(cmdline);
    char *in_file = NULL;
    char *out_file = NULL;
    int append = 0;

    // Input redirection
    char *in_pos = strchr(cmd, '<');
    if (in_pos) {
        *in_pos = '\0';
        in_pos++;
        while (*in_pos == ' ') in_pos++;
        in_file = strtok(in_pos, " \t\n");
        if (in_file) strip_quotes(in_file);
    }

    // Output redirection
    char *out_pos = strstr(cmd, ">>");
    if (out_pos) {
        *out_pos = '\0';
        out_pos += 2;
        while (*out_pos == ' ') out_pos++;
        out_file = strtok(out_pos, " \t\n");
        append = 1;
        if (out_file) strip_quotes(out_file);
    } else {
        out_pos = strchr(cmd, '>');
        if (out_pos) {
            *out_pos = '\0';
            out_pos++;
            while (*out_pos == ' ') out_pos++;
            out_file = strtok(out_pos, " \t\n");
            append = 0;
            if (out_file) strip_quotes(out_file);
        }
    }

    while (*cmd == ' ' || *cmd == '\t') cmd++;

    // Split into argv
    char *argv[64];
    int argc = 0;
    char *token = strtok(cmd, " \t\n");
    while (token && argc < 63) {
        strip_quotes(token);
        argv[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    argv[argc] = NULL;

    if (!argv[0]) { free(cmd); return; }

    // Built-ins (only non-stateful)
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(argv[0], builtin_str[i]) == 0) {
            (*builtin_func[i])(argv);
            free(cmd);
            return;
        }
    }

    // External command
    pid_t pid = fork();
    if (pid == 0) {
        if (in_file) {
            int fd = open(in_file, O_RDONLY);
            if (fd < 0) { perror("open input"); exit(EXIT_FAILURE); }
            dup2(fd, 0); close(fd);
        }
        if (out_file) {
            int fd = append ? open(out_file, O_WRONLY|O_CREAT|O_APPEND, 0644)
                            : open(out_file, O_WRONLY|O_CREAT|O_TRUNC, 0644);
            if (fd < 0) { perror("open output"); exit(EXIT_FAILURE); }
            dup2(fd, 1); close(fd);
        }
        execvp(argv[0], argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        int status;
        waitpid(pid, &status, 0);
    }

    free(cmd);
}

// -------------------------------------------
// Execute command line (handles optional |)
// -------------------------------------------
void execute_line(char *line) {
    char *pipe_pos = strchr(line, '|');
    if (!pipe_pos) {
        execute_simple(line);
        return;
    }

    // Split into two commands
    *pipe_pos = '\0';
    char *cmd1 = line;
    char *cmd2 = pipe_pos + 1;

    while (*cmd1 == ' ' || *cmd1 == '\t') cmd1++;
    while (*cmd2 == ' ' || *cmd2 == '\t') cmd2++;

    // Validate first words
    char cmd1_name[128], cmd2_name[128];
    sscanf(cmd1, "%127s", cmd1_name);
    sscanf(cmd2, "%127s", cmd2_name);
    if (!is_valid_command(cmd1_name) || !is_valid_command(cmd2_name)) {
        printf("Invalid command in pipeline.\n");
        return;
    }

    // Setup pipe
    int fd[2];
    if (pipe(fd) < 0) { perror("pipe"); return; }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(fd[1], 1);
        close(fd[0]); close(fd[1]);
        execute_simple_no_preview(cmd1);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(fd[0], 0);
        close(fd[0]); close(fd[1]);
        execute_simple_no_preview(cmd2);
        exit(0);
    }

    close(fd[0]); close(fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

// -------------------------------------------
// Main execute entry
// -------------------------------------------
int execute(char **args) {
    if (!args[0]) return 1;

    if (!is_valid_command(args[0])) {
        printf("Invalid command: %s\n", args[0]);
        return 1;
    }

    if (!validate_usage(args)) return 1;

    if (strcmp(args[0], "next") != 0) set_last_real_command(args[0]);
    add_to_history(args[0]);

    char *line = join_args(args);
    execute_line(line);
    free(line);

    return 1;
}
