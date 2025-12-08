#ifndef BUILTIN_H
#define BUILTIN_H

int shell_cd(char **args);
int shell_exit(char **args);
int shell_help(char **args);
int shell_next(char **args);
int shell_wlfc(char **args);
int shell_history(char **args);




extern char *builtin_str[];
extern int (*builtin_func[])(char **);

int num_builtins(void);

#endif
