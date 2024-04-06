#include <timer.h>
#include <interrupts.h>
#include <asm.h>
#include <stdbool.h>

volatile size_t timer_ns = 0;
static size_t timer_step = 0;

// interrupt code
void pit_handler()
{
    timer_ns += timer_step;
}
// timer code
size_t read_pit_count(void)
{
    size_t count = 0;

    // al = channel in bits 6 and 7, remaining bits clear
    outb(0x43, 0b0000000);

    count = inb(0x40);       // Low byte
    count |= inb(0x40) << 8; // High byte

    return count;
}
void set_pit_count(size_t count)
{
    outb(0x40, count & 0xFF);          // Low byte
    outb(0x40, (count & 0xFF00) >> 8); // High byte
    return;
}
void timer_init()
{
    set_pit_count(TIMER_STEP);
    timer_step = read_pit_count();
    set_irq_handler(32, pit_handler);
}
void sleep_ns(size_t nanoseconds)
{
    size_t target_ms = timer_ns + nanoseconds;
    while (target_ms > timer_ns)
        asm("hlt");
}
void sleep_ms(size_t milliseconds)
{
    sleep_ns(milliseconds * 1000);
}
void sleep(size_t seconds)
{
    sleep_ns(seconds * 1000000);
}
