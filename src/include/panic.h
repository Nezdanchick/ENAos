#pragma once

#include <math.h>

__attribute__((noreturn)) extern void _panic(const char *error, ...);
#define panic(...) _panic( \
    "PATH       %s:%d\n"   \
    "FUNCTION   %s\n"      \
    "MESSAGE    %s\n",     \
    __FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)
