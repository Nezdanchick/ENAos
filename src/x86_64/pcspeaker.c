#include <pcspeaker.h>
#include <timer.h>
#include <asm.h>

void play_sound(uint32_t frequence)
{
    if (frequence == 0)
        return;

    uint32_t div = 1193180 / frequence;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t)(div));
    outb(0x42, (uint8_t)(div >> 8));

    uint8_t tmp = inb(0x61);
    if (tmp != (tmp | 3))
    {
        outb(0x61, tmp | 3);
    }
}
void mute()
{
    uint8_t tmp = inb(0x61) & 0xFC;

    outb(0x61, tmp);
}
void beep(int frequence, int length)
{
    play_sound(frequence);
    sleep_ms(length);
    mute();
}
