#include <interrupts.h>
#include <stdbool.h>

#define KERNEL_CODE_SEGMENT_SELECTOR 0x08

extern uint64_t isr_table[256];

typedef struct
{
    uint16_t offset_0_15;
    uint16_t segment_selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t offset_16_31;
    uint32_t offset_32_63;
    uint32_t reserved;
} __attribute__((__packed__)) idt_entry_t;

typedef struct __attribute__((__packed__))
{
    const uint16_t limit;
    const idt_entry_t *data;
} idt_pointer_t;

const int PRESENT_OFFSET = 7;
const int DPL_OFFSET = 5;
const int INTERRUPT_TYPE_OFFSET = 0;

idt_entry_t idt_data[256];
const idt_pointer_t idt = {
    .data = idt_data,
    .limit = sizeof(idt_data) - 1,
};

idt_entry_t gen_interrupt_descriptor(uint64_t offset, uint8_t dpl)
{
    bool is_present = true;
    bool is_trap_gate = false;
    return (idt_entry_t){
        .offset_0_15 = offset & 0xFFFF,
        .segment_selector = KERNEL_CODE_SEGMENT_SELECTOR,
        .ist = 0,
        .flags = ((is_present & 1) << PRESENT_OFFSET) |
                 ((dpl & 0b11) << DPL_OFFSET) |
                 ((is_trap_gate & 1) << INTERRUPT_TYPE_OFFSET) |
                 0b1110,
        .offset_16_31 = (offset >> 16) & 0xFFFF,
        .offset_32_63 = (offset >> 32) & 0xFFFFFFFF,
        .reserved = 0};
}

#define SET_USER_ISR(i) \
    idt_data[i] = gen_interrupt_descriptor(isr_table[i], 3)

#define SET_KERNEL_ISR(i) \
    idt_data[i] = gen_interrupt_descriptor(isr_table[i], 0)

#define SET_ERROR_ISR(i) \
    idt_data[i] = gen_interrupt_descriptor(isr_table[i], 3)

void idt_init(void)
{
    for (int i = 0; i < 256; i++)
    {
        if (i < 32)
            SET_ERROR_ISR(i);
        else if (i != 128)
            SET_KERNEL_ISR(i);
    }
    SET_USER_ISR(128);

    __asm__ volatile("lidt %0" : : "m"(idt));
}