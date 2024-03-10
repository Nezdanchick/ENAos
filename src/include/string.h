#pragma once

#include <stddef.h>

extern char* strcpy(char* destination, const char* source);
extern void *memset(void *buffer, int value, size_t size);
extern char *strext(char *destination, char *source, char attribute);
extern size_t strlen(const char *string);
extern char *itoa(size_t value, char *string, int base);
