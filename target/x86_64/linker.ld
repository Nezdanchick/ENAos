OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(start)

mb_magic = 0xe85250d6;
mb_arch = 0;
mb_length = 24 * 8;

SECTIONS
{
	.multiboot :
	{
		LONG(mb_magic);
		LONG(mb_arch);
		LONG(mb_length);
		LONG(- (mb_magic + mb_length));
		SHORT(0);
		SHORT(0);
		LONG(8);
	}
	.text :
	{
		*(.text)
	}
}
