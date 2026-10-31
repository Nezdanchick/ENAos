#include <stdint.h>
#include "serial.h"
#include "screen.h"

void kmain(uint32_t multiboot_addr, uint32_t multiboot_magic)
{
    (void)multiboot_addr;
    (void)multiboot_magic;

    serial_init(COM1);
    screen_init();

    serial_write("Hello, world from ENAos!\r\n");
    screen_write("Hello, world from ENAos!\n");

    while (1) {
        __asm__ volatile ("hlt");
    }
}
