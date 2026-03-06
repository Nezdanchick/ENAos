#include <timer.h>
#include <stdbool.h>
#include <interrupts.h>
#include <io.h>

#define MAX_CALLBACKS 32

volatile size_t timer_ns = 0;
static size_t timer_step = 0;

typedef struct {
    timer_callback_t callback;
    size_t interval_ns;
    size_t last_call_ns;
    bool active;
} timer_callback_entry_t;

static timer_callback_entry_t callbacks[MAX_CALLBACKS];
static size_t callback_count = 0;

// interrupt code
void pit_handler()
{
    timer_ns += timer_step;
    
    for (size_t i = 0; i < callback_count; i++)
    {
        if (callbacks[i].active && callbacks[i].callback != NULL)
        {
            if (timer_ns - callbacks[i].last_call_ns >= callbacks[i].interval_ns)
            {
                callbacks[i].last_call_ns = timer_ns;
                callbacks[i].callback();
            }
        }
    }
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
void init_timer()
{
    set_pit_count(TIMER_STEP);
    timer_step = read_pit_count();
    set_irq_handler(32, pit_handler);
}
void sleep_ns(size_t nanoseconds)
{
    size_t target_ms = timer_ns + nanoseconds;
    while (target_ms > timer_ns)
        __asm__("hlt");
}
void sleep_ms(size_t milliseconds)
{
    sleep_ns(milliseconds * 1000);
}
void sleep(size_t seconds)
{
    sleep_ns(seconds * 1000000);
}
bool timer_add_callback(timer_callback_t callback, size_t interval_ms)
{
    if (callback == NULL || callback_count >= MAX_CALLBACKS)
        return false;
    
    for (size_t i = 0; i < callback_count; i++)
    {
        if (callbacks[i].callback == callback)
            return false;
    }
    
    callbacks[callback_count].callback = callback;
    callbacks[callback_count].interval_ns = interval_ms * 1000;
    callbacks[callback_count].last_call_ns = timer_ns;
    callbacks[callback_count].active = true;
    callback_count++;
    
    return true;
}

bool timer_remove_callback(timer_callback_t callback)
{
    if (callback == NULL)
        return false;
    
    for (size_t i = 0; i < callback_count; i++)
    {
        if (callbacks[i].callback == callback)
        {
            callbacks[i].active = false;
            callbacks[i].callback = NULL;
            
            if (i == callback_count - 1)
            {
                callback_count--;
            }
            return true;
        }
    }
    
    return false;
}
