target=$(output)/boot/kernel.bin

all: $(target)
	@mkdir -p $(output)/boot/grub
	@cp grub.cfg $(output)/boot/grub

include $(module_makefile)
