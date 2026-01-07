#pragma once

#include <stddef.h>
#include <string.h>
#include <memory.h>

extern void init_alias();
extern void set_alias(char *alias, char *command);
extern char *get_alias(char *command);
extern void del_alias(char *command);
extern void list_aliases();
