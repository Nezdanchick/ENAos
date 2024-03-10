#include <screen.h>
#include <string.h>
#include <stdbool.h>

void main()
{
    screenClear();
    
    for (size_t i = 0; true; i++)
    {
        screenSetColor(i % 16, (i + 8) % 16);
        screenWrite("Hello, 64 bit world!");
        screenWrite(" ");
        screenWrite(itoa(i, NULL, 10));
        screenWrite(" ");
        screenWriteLine(itoa(getPosition(), NULL, 10));
        for (long sleep_i = 0; sleep_i < 100000; sleep_i++)
            screenSetColor(SCREEN_COLOR_WHITE, SCREEN_COLOR_BLACK); // don't optimized
    }
}
