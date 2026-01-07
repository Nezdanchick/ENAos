#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

extern char *strncpy(char *destination, const char *source, size_t len);
extern char* strcpy(char* destination, const char* source);
extern int strcmp(const char *a, const char *b);
extern int strncmp(const char *a, const char *b, size_t length);
extern char *strext(char *destination, char *source, char attribute);
extern char *strtok(char *string, const char *separator);
extern size_t strlen(const char *string);
extern size_t strcount(const char *string, char character);
extern char *itoa(size_t value, char *string, int base);
extern int atoi(const char *string);
