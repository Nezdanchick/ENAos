#include <panic.h>
#include <screen.h>

void panic(char *error) // TODO print other info about panic
{
    screenClear();
    screenSetColor(SCREEN_COLOR_RED, SCREEN_COLOR_BLACK);
    screenWrite(error);

    asm("cli");
    asm("hlt");
halt:
    goto halt;
}