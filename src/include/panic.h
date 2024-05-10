#pragma once

__attribute__((noreturn)) // halt
extern void panic(const char *error, ...);
