#pragma once

#include <stdarg.h>

typedef char *stdin();
typedef void stdout(char *);

typedef int getpos();
typedef void setpos(int, int);

typedef void clear();

extern int terminal_x;
extern int terminal_y;

extern stdout *terminal_output;

extern getpos *terminal_getpos;
extern setpos *terminal_setpos;

extern clear *terminal_clear;

extern void set_stdout(stdout *func);
extern void set_getpos(getpos *func);
extern void set_setpos(setpos *func);
extern void set_clear(clear *func);

#define newline() terminal_setpos(0, terminal_getpos() / terminal_x + 1);
#define carriage_return() terminal_setpos(0, terminal_getpos() / terminal_x);

extern void printf(const char *fmt, ...);
extern void vprintf(const char *fmt, va_list args);
extern void putchar(char character);
extern char *gets(char *string);
