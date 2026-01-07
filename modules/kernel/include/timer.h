#pragma once

#include <stddef.h>
#include <stdbool.h>

// how often the timer fires in nanoseconds
#define TIMER_STEP 10000

typedef void (*timer_callback_t)(void);

extern void init_timer();
extern void sleep_ns(size_t nanoseconds);
extern void sleep_ms(size_t milliseconds);
extern void sleep(size_t seconds);

bool timer_add_callback(timer_callback_t callback, size_t interval_ms);
bool timer_remove_callback(timer_callback_t callback);
