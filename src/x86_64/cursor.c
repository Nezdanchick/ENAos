#include <cursor.h>
#include <stdio.h>
#include <io.h>

void cursor_init()
{
    outb(0x3D4, 0x0B); // set the cursor end line to 15
    outb(0x3D5, 0x0F);

    outb(0x3D4, 0x0A); // set the cursor start line to 14 and enable cursor visibility
    outb(0x3D5, 0x0E);
}
void cursor_disable()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}
void cursor_set(int x, int y)
{
    uint16_t pos = y * terminal_width + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}