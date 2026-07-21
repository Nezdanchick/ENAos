#include <init.h>

void module_load(struct multiboot_tag_module *module)
{
    printf("Loading module at 0x%x of size 0x%x bytes\ncmd: %s\n", module->mod_start, module->mod_end - module->mod_start, module->cmdline);
    if (strcmp(module->cmdline, "initramfs") == 0)
    {
        init_cpio((uint8_t *)(uintptr_t)module->mod_start);
    }
}

void init(uint32_t multiboot_addr, uint32_t multiboot_magic)
{
    init_serial(COM1);

    init_pmm();
    init_screen();
    init_vfs();

    init_interrupts();

    init_timer();
    init_keyboard();

    if (multiboot_magic != MULTIBOOT2_BOOTLOADER_MAGIC)
        panic("Multiboot2 magic (0x%x) is incorrect\n", multiboot_magic);

    if (multiboot_addr & 0b111)
        panic("Unaligned mbi: 0x%x\n", multiboot_addr);

    struct multiboot_tag_framebuffer *fbtag = {0};

    for (struct multiboot_tag *tag = (struct multiboot_tag *)((uintptr_t)(multiboot_addr + 8));
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
    {
        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
            fbtag = (struct multiboot_tag_framebuffer *)tag;
            break;
        case MULTIBOOT_TAG_TYPE_MODULE:
            module_load((struct multiboot_tag_module *)tag);
            break;
        }
    }

    init_framebuffer(fbtag);

    if (fbtag && fbtag->common.framebuffer_addr != 0xb8000)
        init_graphics_terminal();
}
