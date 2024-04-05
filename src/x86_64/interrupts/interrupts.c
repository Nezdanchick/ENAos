#include <interrupts.h>

void interrupts_init() {
    idt_init();
    pic_init();
}