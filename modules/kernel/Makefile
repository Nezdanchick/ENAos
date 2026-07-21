target=$(iso)/boot/kernel.bin

all: $(target)
	@mkdir -p $(iso)/boot/limine
	@cp limine.conf $(iso)/boot/limine.conf

	@mkdir -p $(output)/sys/resources
	@cp help.txt $(output)/sys/resources
	@cp ena.bmp $(output)/sys/resources
	@cp init.ena $(output)/sys
	@find . -maxdepth 1 -name "*.ena" ! -name "init.ena" -exec cp {} $(output) \;

include $(module_makefile)
