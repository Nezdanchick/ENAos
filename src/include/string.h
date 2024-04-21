#pragma once

#include <stddef.h>
#include <stdbool.h>

extern char* strcpy(char* destination, const char* source);
extern bool strcmp(const char *a, const char *b);
extern char *strext(char *destination, char *source, char attribute);
extern size_t strlen(const char *string);
extern char *itoa(size_t value, char *string, int base);
