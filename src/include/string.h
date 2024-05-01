#pragma once

#include <stddef.h>
#include <stdbool.h>

extern char* strcpy(char* destination, const char* source);
extern bool strcmp(const char *a, const char *b);
extern bool strncmp(const char *a, const char *b, size_t length);
extern char *strext(char *destination, char *source, char attribute);
extern char *strtok(char *string, const char *separator);
extern size_t strlen(const char *string);
extern char *itoa(int value, char *string, int base);
extern int atoi(const char *string);
