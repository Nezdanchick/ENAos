#include <alias.h>
#include <stdio.h>
#include <memory.h>

#define MAX_ALIASES 64
#define MAX_ALIAS_LEN 64
#define MAX_CMD_LEN 128

static char alias_names[MAX_ALIASES][MAX_ALIAS_LEN];
static char alias_commands[MAX_ALIASES][MAX_CMD_LEN];
static int alias_count = 0;

void init_alias()
{
    alias_count = 0;
}

void set_alias(char *alias, char *command)
{
    if (strlen(alias) == 0 || strlen(command) == 0)
        return;
    
    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(alias_names[i], alias) == 0)
        {
            strcpy(alias_commands[i], command);
            return;
        }
    }
    
    if (alias_count >= MAX_ALIASES)
        return;
    
    strcpy(alias_names[alias_count], alias);
    strcpy(alias_commands[alias_count], command);
    alias_count++;
}

char *get_alias(char *command)
{
    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(alias_names[i], command) == 0)
        {
            return alias_commands[i];
        }
    }
    return NULL;
}

void del_alias(char *command)
{
    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(alias_names[i], command) == 0)
        {
            for (int j = i; j < alias_count - 1; j++)
            {
                strcpy(alias_names[j], alias_names[j + 1]);
                strcpy(alias_commands[j], alias_commands[j + 1]);
            }
            alias_count--;
            break;
        }
    }
}

void list_aliases()
{
    if (alias_count == 0)
    {
        printf("No aliases defined\n");
        return;
    }
    
    for (int i = 0; i < alias_count; i++)
    {
        printf("%s = %s\n", alias_names[i], alias_commands[i]);
    }
}
