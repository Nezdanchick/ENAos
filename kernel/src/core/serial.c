#include <stdarg.h>
#include "serial.h"
#include "io.h"

static uint16_t serial_port;

static int is_transmit_empty(void)
{
    return inb(serial_port + 5) & 0x20;
}

int serial_init(enum Port port)
{
    serial_port = port;
    outb(serial_port + 1, 0x00);
    outb(serial_port + 3, 0x80);
    outb(serial_port + 0, 0x03);
    outb(serial_port + 1, 0x00);
    outb(serial_port + 3, 0x03);
    outb(serial_port + 2, 0xC7);
    outb(serial_port + 4, 0x0B);
    outb(serial_port + 4, 0x1E);
    outb(serial_port + 0, 0x60);

    if (inb(serial_port) != 0x60)
    {
        return 1;
    }

    outb(serial_port + 4, 0x0F);
    return 0;
}

void serial_putc(char c)
{
    while (is_transmit_empty() == 0)
        ;
    outb(serial_port, c);
}

void serial_write(char *string)
{
    while (*string != '\0')
        serial_putc(*string++);
}

static void serial_print_num(uint64_t val, int base)
{
    const char digits[] = "0123456789abcdef";
    char buf[32];
    int i = 0;

    if (base == 16)
    {
        serial_write("0x");
    }
    else if ((int64_t)val < 0 && base == 10)
    {
        serial_putc('-');
        val = -val;
    }

    do
    {
        buf[i++] = digits[val % base];
        val /= base;
    } while (val);

    while (i > 0)
    {
        serial_putc(buf[--i]);
    }
}

void serial_formatted(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            if (*fmt == 's')
            {
                const char *s = va_arg(args, const char *);
                serial_write((char *)(s ? s : "(null)"));
            }
            else if (*fmt == 'x')
            {
                serial_print_num(va_arg(args, uint64_t), 16);
            }
            else if (*fmt == 'd')
            {
                serial_print_num(va_arg(args, int64_t), 10);
            }
            else if (*fmt == '%')
            {
                serial_putc('%');
            }
        }
        else
        {
            serial_putc(*fmt);
        }
        fmt++;
    }

    va_end(args);
}
