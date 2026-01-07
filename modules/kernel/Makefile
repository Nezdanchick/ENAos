target=$(output)/boot/kernel.bin

all: $(target)
	@mkdir -p $(output)/boot/grub
	@cp grub.cfg $(output)/boot/grub
	@cp ena.bmp $(output)

include $(module_makefile)
