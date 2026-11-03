ENTRY(_start)

OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

USER_BASE = 0x0000000000400000;
PAGE_SIZE = 4096;

SECTIONS
{
    . = USER_BASE;

    .text : ALIGN(PAGE_SIZE) {
        *(.text .text.*)
    }

    .rodata : ALIGN(PAGE_SIZE) {
        *(.rodata .rodata.*)
    }

    .data : ALIGN(PAGE_SIZE) {
        *(.data .data.*)
    }

    .bss : ALIGN(PAGE_SIZE) {
        *(COMMON)
        *(.bss .bss.*)
    }

    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
        *(.comment)
    }
}
