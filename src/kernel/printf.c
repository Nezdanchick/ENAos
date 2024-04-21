#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <pcspeaker.h>

void newline()
{
    terminal_setpos(0, terminal_getpos() / terminal_x + 1);
}
void carriage_return()
{
    terminal_setpos(0, terminal_getpos() / terminal_x);
}
void putchar(char character)
{
    terminal_output((char[2]){character, '\0'});
}
void printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    for (char prev_char = '\0'; *fmt != '\0'; ++fmt)
    {
        if (prev_char == '%')
        {
            switch (*fmt)
            {
            case 'd':
                terminal_output(itoa(va_arg(args, int), NULL, 10));
                break;
            case 'x':
                terminal_output(itoa(va_arg(args, int), NULL, 16));
                break;
            case 'c':
                putchar(va_arg(args, int));
                break;
            case 's':
                terminal_output(va_arg(args, char *));
                break;
            default:
                terminal_output((char[3]){'%', *fmt, '\0'});
                break;
            }
        }
        else
        {
            switch (*fmt)
            {
            case '\r':
                carriage_return();
                break;
            case '\n':
                newline();
                break;
            case '\t':
                terminal_output("    ");
                break;
            case '\b':
                beep(800, 100);
                break;
            case '\\':
                terminal_output("\\");
                break;
            default:
                if (*fmt != '%' && *fmt != '\\')
                    putchar(*fmt);
                break;
            }
        }
        prev_char = *fmt;
    }

    va_end(args);
}