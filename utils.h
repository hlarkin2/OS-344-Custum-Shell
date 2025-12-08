#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>


void set_last_real_command(const char *cmd);
const char* get_last_real_command();
int is_valid_command(char *cmd);
int validate_usage(char **args);
int command_in_path(const char *cmd);
char *join_args(char **args);
int confirm_execution();
void add_to_history(const char *cmd);


// History struct and variables
typedef struct {
    char command[64];
    char description[128];
} CommandHistory;



#define HISTORY_SIZE 10


extern CommandHistory history[HISTORY_SIZE];
extern int history_count;

#endif
