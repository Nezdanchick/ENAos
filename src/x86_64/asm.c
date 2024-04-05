#include <asm.h>

void outb(uint16_t port, uint8_t value)
{
    asm("outb %0, %1" : : "a"(value), "Nd"(port));
}
uint8_t inb(uint16_t port)
{
    uint8_t result;
    asm("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}