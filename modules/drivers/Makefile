drivers:=$(shell find . -maxdepth 1 -type d ! -name "." ! -name ".*" | sed 's|./||')

$(drivers):
	@mkdir -p $(output)/drivers
	@echo Building driver $@
	@make -C $@ \
		output=$(output) module_makefile=$(module_makefile) \
		global_inc=$(global_inc) global_obj=$(global_obj)

.PHONY: $(drivers)
