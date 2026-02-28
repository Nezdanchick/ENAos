#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <serial.h>
#include <pcspeaker.h>

int putchar(int c)
{
    unsigned char ch = (unsigned char)c;

    switch (ch)
    {
        case '\n':
            putserial('\n');
            terminal_x = 0;
            terminal_y++;
            terminal_check_position();
            return ch;

        case '\r':
            putserial('\r');
            terminal_x = 0;
            return ch;

        case '\t':
        {
            int spaces = 4 - (terminal_x % 4);
            for (int i = 0; i < spaces; i++)
                putchar(' ');
            return ch;
        }

        case '\b':
            if (terminal_backspace)
                terminal_backspace();
            return ch;

        default:
        {
            char buf[2] = { ch, 0 };

            putserial(ch);

            if (terminal_output)
                terminal_output(buf);

            return ch;
        }
    }
}

static int print_padding(int width, int len, char pad)
{
    int count = 0;
    while (len < width)
    {
        putchar(pad);
        len++;
        count++;
    }
    return count;
}

static int print_unsigned(uint64_t value, int base, int width, int zero_pad)
{
    char buffer[32];
    int i = 0;
    int count = 0;

    if (value == 0)
        buffer[i++] = '0';
    else
        while (value)
        {
            int digit = value % base;
            buffer[i++] = (digit < 10) ? '0' + digit : 'a' + digit - 10;
            value /= base;
        }

    count += print_padding(width, i, zero_pad ? '0' : ' ');

    while (i--)
    {
        putchar(buffer[i]);
        count++;
    }

    return count;
}

static int print_signed(int64_t value, int width, int zero_pad)
{
    int count = 0;

    if (value < 0)
    {
        putchar('-');
        count++;
        value = -value;
        if (width > 0)
            width--;
    }

    return count + print_unsigned((uint64_t)value, 10, width, zero_pad);
}

int vprintf(const char *fmt, va_list args)
{
    int count = 0;

    while (*fmt)
    {
        if (*fmt != '%')
        {
            putchar(*fmt++);
            count++;
            continue;
        }

        fmt++;

        int zero_pad = 0;
        int width = 0;
        int long_flag = 0;

        if (*fmt == '0')
        {
            zero_pad = 1;
            fmt++;
        }

        while (*fmt >= '0' && *fmt <= '9')
        {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }

        if (*fmt == 'l')
        {
            long_flag = 1;
            fmt++;
        }

        switch (*fmt)
        {
        case '%':
            putchar('%');
            count++;
            break;

        case 'd':
            if (long_flag)
                count += print_signed(va_arg(args, int64_t), width, zero_pad);
            else
                count += print_signed(va_arg(args, int), width, zero_pad);
            break;

        case 'u':
            if (long_flag)
                count += print_unsigned(va_arg(args, uint64_t), 10, width, zero_pad);
            else
                count += print_unsigned(va_arg(args, unsigned int), 10, width, zero_pad);
            break;

        case 'x':
            if (long_flag)
                count += print_unsigned(va_arg(args, uint64_t), 16, width, zero_pad);
            else
                count += print_unsigned(va_arg(args, unsigned int), 16, width, zero_pad);
            break;

        case 'p':
            putchar('0');
            putchar('x');
            count += 2;
            count += print_unsigned((uintptr_t)va_arg(args, void*), 16, sizeof(void*) * 2, 1);
            break;

        case 'c':
        {
            char c = (char)va_arg(args, int);
            putchar(c);
            count++;
            break;
        }

        case 's':
        {
            const char *s = va_arg(args, const char*);
            if (!s)
                s = "(null)";

            int len = 0;
            const char *tmp = s;
            while (*tmp++) len++;

            count += print_padding(width, len, ' ');

            while (*s)
            {
                putchar(*s++);
                count++;
            }
            break;
        }

        default:
            putchar('%');
            putchar(*fmt);
            count += 2;
            break;
        }

        fmt++;
    }

    return count;
}

int printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int count = vprintf(fmt, args);
    va_end(args);
    return count;
}