#include <stdbool.h>
#include <string.h>
#include <screen.h>
#include <pcspeaker.h>
#include <asm.h>
#include <interrupts.h>

void kb_handler()
{
    uint8_t scancode = inb(0x60);
    screenSetColor(scancode % 15 + 1, SCREEN_COLOR_BLACK);
    screenWrite("Key scancode: ");
    screenWriteLine(itoa(scancode, NULL, 10));
}
uint64_t ms = 0;
void pit_handler()
{
    ms += 20;
}

extern uint16_t idt_table;
void main()
{
    screenClear();
    interrupts_init();
    set_irq_handler(32, pit_handler);
    set_irq_handler(33, kb_handler);
    asm("sti");

    screenWriteLine("Welcome to nandos!");
    
    while (ms <= 1000)  // wait
    {
        setPosition(0, 2);
        screenWrite("Milliseconds: ");
        screenWriteLine(itoa(ms, NULL, 10));
    }
}
