target=$(iso)/boot/ena

all: $(target)
	@mkdir -p $(iso)/boot/limine
	@cp limine.conf $(iso)/boot/limine.conf

	@mkdir -p $(output)/sys/resources
	@cp help.txt $(output)/sys/resources
	@cp ena.bmp $(output)/sys/resources

include $(module_makefile)
