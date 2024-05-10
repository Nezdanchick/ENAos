OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(start)

SECTIONS
{
	kernel_start = .;
	
	.multiboot :
	{
		KEEP(*(.multiboot_header))
	}
	.text :
	{
		KEEP(*(.text))
	}
	.data :
	{
		KEEP(*(.data))
	}
	.bss :
	{
		KEEP(*(.bss))
	}

	kernel_end = .;
}
