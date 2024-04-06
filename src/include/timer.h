#pragma once

#include <stddef.h>

// how often the timer fires in nanoseconds
#define TIMER_STEP 500

extern void timer_init();
extern void sleep_ns(size_t nanoseconds);
extern void sleep_ms(size_t milliseconds);
extern void sleep(size_t seconds);
