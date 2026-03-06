#pragma once

#include <stdarg.h>

__attribute__((sysv_abi, noinline, noreturn)) 
void _panic(const char *file, int line, const char *func, const char *msg, ...);

#define panic(msg, ...) \
    _panic(__FILE__, __LINE__, __PRETTY_FUNCTION__, msg, ##__VA_ARGS__)