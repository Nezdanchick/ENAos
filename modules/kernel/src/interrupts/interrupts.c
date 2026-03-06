#include <interrupts.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>

void init_interrupts() {
    idt_init();
    pic_init();
    __asm__("sti");
}