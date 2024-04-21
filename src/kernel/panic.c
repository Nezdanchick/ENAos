#include <panic.h>
#include <screen.h>
#include <stdio.h>

void panic(char *error) // TODO print other info about panic
{
    terminal_clear();
    screen_set_color(SCREEN_COLOR_RED, SCREEN_COLOR_BLACK);
    printf(error);

    asm("cli");
    asm("hlt");
halt:
    goto halt;
}