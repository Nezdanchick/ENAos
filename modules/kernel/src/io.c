#include <io.h>

// byte
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
// word
void outw(uint16_t port, uint16_t value)
{
    asm("outw %0, %1" : : "a"(value), "Nd"(port));
}
uint16_t inw(uint16_t port)
{
    uint16_t result;
    asm("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
// double word
void outl(uint16_t port, uint32_t value)
{
    asm("outl %0, %1" : : "a"(value), "Nd"(port));
}
uint32_t inl(uint16_t port)
{
    uint32_t result;
    asm("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
// wait
void io_wait(void)
{
    outb(0x80, 0);
}