ENTRY(start)

OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

PHYS_BASE = 0x00100000;
VIRT_BASE = 0xFFFFFFFF80000000;
PAGE_SIZE = 4096;

SECTIONS
{
    . = PHYS_BASE;

    .boot : ALIGN(PAGE_SIZE) {
        KEEP(*(.multiboot_header))
        *(.boot.text)
        *(.boot.data)
        *(.boot.rodata)
        *(.boot.bss)
    }

    . = ALIGN(PAGE_SIZE);
    . += VIRT_BASE;

    .text : AT(ADDR(.text) - VIRT_BASE) ALIGN(PAGE_SIZE) {
        *(.text .text.*)
    }

    .rodata : AT(ADDR(.rodata) - VIRT_BASE) ALIGN(PAGE_SIZE) {
        *(.rodata .rodata.*)
    }

    .data : AT(ADDR(.data) - VIRT_BASE) ALIGN(PAGE_SIZE) {
        *(.data .data.*)
    }

    .bss : AT(ADDR(.bss) - VIRT_BASE) ALIGN(PAGE_SIZE) {
        sbss = .;
        *(COMMON)
        *(.bss .bss.*)
        ebss = .;
    }

    kernel_end = . - VIRT_BASE;

    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
        *(.comment)
    }
}
