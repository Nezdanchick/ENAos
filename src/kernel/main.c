#include <stdbool.h>
#include <string.h>
#include <screen.h>
#include <pcspeaker.h>
#include <asm.h>
#include <interrupts.h>
#include <timer.h>

void kb_handler()
{
    uint8_t scancode = inb(0x60);
    screenSetColor(scancode % 15 + 1, SCREEN_COLOR_BLACK);
    screenWrite("Key scancode: ");
    screenWriteLine(itoa(scancode, NULL, 10));
}
void main()
{
    screenClear();
    interrupts_init();
    set_irq_handler(33, kb_handler);
    timer_init();
    asm("sti");

    screenWriteLine("Welcome to nandos!");
    beep(500, 100);
    beep(1500, 100);
    beep(100, 200);
    screenWriteLine("We have timed beep!");
}
