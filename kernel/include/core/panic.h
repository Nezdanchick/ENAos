#pragma once

#include "cpu_context.h"

__attribute__((sysv_abi, noinline, noreturn))
void _panic(const char *file, int line, const char *func, const char *msg, const cpu_context_t *context);

#define panic(msg) \
    _panic(__FILE__, __LINE__, __PRETTY_FUNCTION__, msg, (void *)0)

#define panic_context(msg, context) \
    _panic(__FILE__, __LINE__, __PRETTY_FUNCTION__, msg, context)
