#pragma once

#include <stdbool.h>
#include <stddef.h>

extern void *logo_bmp;
extern char *evaluate(char *command);
extern void shell();

char *eval_string(const char *arg);
char **get_args(char *string, size_t start, char separator, int count);
char **eval_args(char *string, size_t start, char separator, int count);
void free_args(char **args);

extern int recursion_depth;
extern int recursion_limit;
char* execute_builtin(char *command);
