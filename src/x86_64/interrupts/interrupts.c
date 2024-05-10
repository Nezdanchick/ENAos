#include <interrupts.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>

void interrupts_init() {
    idt_init();
    pic_init();
}