#include <init.h>

void init(uint64_t multiboot_addr, uint64_t multiboot_magic)
{
    screen_init();
    serial_init(COM1);
    interrupts_init();
    asm("sti");
    
    timer_init();
    keyboard_init();
    
    pmm_init();

    if (multiboot_magic != MULTIBOOT2_BOOTLOADER_MAGIC)
        panic("Multiboot2 magic (0x%x) is incorrect\n", multiboot_magic);

    if (multiboot_addr & 0b111)
        panic("Unaligned mbi: 0x%x\n", multiboot_addr);

    struct multiboot_tag_framebuffer *fbtag = {0};

    for (struct multiboot_tag *tag = (struct multiboot_tag *)(multiboot_addr + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
    {
        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
            fbtag = (struct multiboot_tag_framebuffer *)tag;
            break;
        }
    }
    fb_init(fbtag);

    if (fbtag->common.framebuffer_addr != 0xb8000)
    {
        fb_terminal_init();
        cursor_disable();
    }
    terminal_clear();
}
