#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <pcspeaker.h>
#include <serial.h>

void putchar(char character)
{
    putserial(character);
    terminal_output((char[2]){character, '\0'});
}
void puts(char *string)
{
    serial_write(string);
    terminal_output(string);
}
void vprintf(const char *fmt, va_list args)
{
    for (char prev_char = '\0'; *fmt != '\0'; ++fmt)
    {
        if (prev_char == '%')
        {
            switch (*fmt)
            {
            case 'd':
                puts(itoa(va_arg(args, int), NULL, 10));
                break;
            case 'x':
                puts(itoa(va_arg(args, int), NULL, 16));
                break;
            case 'c':
                putchar(va_arg(args, int));
                break;
            case 's':
                puts(va_arg(args, char *));
                break;
            default:
                puts((char[3]){'%', *fmt, '\0'});
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
                puts("    ");
                break;
            case '\b':
                beep(800, 100);
                break;
            case '\\':
                puts("\\");
                break;
            default:
                if (*fmt != '%' && *fmt != '\\')
                    putchar(*fmt);
                break;
            }
        }
        prev_char = *fmt;
    }
}
