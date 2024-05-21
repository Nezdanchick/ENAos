#pragma once

#include <stdarg.h>

typedef void backspace();
typedef void clear();
typedef void scroll();
typedef void stdout(char *string);
typedef void setpos(int x, int y); // set cursor position

extern int terminal_x;
extern int terminal_y;
extern int terminal_width;
extern int terminal_height;

extern backspace *terminal_backspace;
extern clear *terminal_clear;
extern scroll *terminal_scroll;
extern stdout *terminal_output;
extern setpos *cursor_setpos;

extern void set_backspace(backspace *func);
extern void set_clear(clear *func);
extern void set_scroll(scroll *func);
extern void set_stdout(stdout *func);
extern void set_cursor_setpos(setpos *func);

extern int terminal_getpos();
extern void terminal_setpos(int collumn, int row);
extern void terminal_check_position();

extern void printf(const char *fmt, ...);
extern void vprintf(const char *fmt, va_list args);
extern void putchar(char character);
extern char *gets(char *string);
