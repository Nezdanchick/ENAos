#include <history.h>
#include <string.h>

#define MAX_HISTORY 100
#define MAX_CMD_LEN 256

static char history[MAX_HISTORY][MAX_CMD_LEN];
static size_t history_size = 0;
static size_t history_pos = 0;
static char current_buffer[MAX_CMD_LEN];

void init_history()
{
    history_size = 0;
    history_pos = 0;
    memset(current_buffer, 0, MAX_CMD_LEN);
}

void history_add(const char *command)
{
    if (strlen(command) == 0)
        return;
    
    if (history_size > 0 && strcmp(history[history_size - 1], command) == 0)
        return;
    
    if (history_size < MAX_HISTORY)
    {
        strncpy(history[history_size], command, MAX_CMD_LEN - 1);
        history[history_size][MAX_CMD_LEN - 1] = '\0';
        history_size++;
    }
    else
    {
        for (size_t i = 0; i < MAX_HISTORY - 1; i++)
        {
            strcpy(history[i], history[i + 1]);
        }
        strncpy(history[MAX_HISTORY - 1], command, MAX_CMD_LEN - 1);
        history[MAX_HISTORY - 1][MAX_CMD_LEN - 1] = '\0';
    }
    
    history_pos = history_size;
}

const char *history_get_prev()
{
    if (history_size == 0)
        return NULL;
    
    if (history_pos > 0)
        history_pos--;
    
    if (history_pos < history_size)
        return history[history_pos];
    
    return NULL;
}

const char *history_get_next()
{
    if (history_size == 0)
        return NULL;
    
    if (history_pos < history_size)
        history_pos++;
    
    if (history_pos < history_size)
        return history[history_pos];
    
    history_pos = history_size;
    return NULL;
}

void history_reset_position()
{
    history_pos = history_size;
}

size_t history_count()
{
    return history_size;
}
