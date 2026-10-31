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
