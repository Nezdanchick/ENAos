target=$(output)/boot/kernel.bin

all: $(target)
	@mkdir -p $(output)/boot/limine
	@cp limine.conf $(output)/boot/limine.conf
	@cp ena.bmp $(output)

include $(module_makefile)
