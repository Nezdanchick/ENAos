#pragma once

#include <stddef.h>

extern void init_history();
extern void history_add(const char *command);
extern const char *history_get_prev();
extern const char *history_get_next();
extern void history_reset_position();
extern size_t history_count();
