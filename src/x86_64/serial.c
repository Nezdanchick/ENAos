#include <serial.h>
#include <io.h>

int serialport;

int serial_init(enum Port port)
{
    serialport = port;
    outb(serialport + 1, 0x00); // Disable all interrupts
    outb(serialport + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(serialport + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(serialport + 1, 0x00); //                  (hi byte)
    outb(serialport + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(serialport + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(serialport + 4, 0x0B); // IRQs enabled, RTS/DSR set
    outb(serialport + 4, 0x1E); // Set in loopback mode, test the serial chip
    outb(serialport + 0, 0x60); // Test serial chip (send byte 0x60 and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (inb(serialport) != 0x60)
    {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(serialport + 4, 0x0F);
    return 0;
}
int serial_received()
{
    return inb(serialport + 5) & 1;
}
int is_transmit_empty()
{
    return inb(serialport + 5) & 0x20;
}
char serial_read()
{
    while (serial_received() == 0)
        ;
    return inb(serialport);
}
void putserial(char character)
{
    while (is_transmit_empty() == 0)
        ;
    outb(serialport, character);
}
void serial_write(char *string)
{
    while (*string != '\0')
        putserial(*string++);
}
