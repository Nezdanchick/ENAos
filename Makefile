iso_path=bin/ENAos.iso

build_output:=iso
module_makefile:=module.Makefile

priority_modules:=test global

modules_all:=$(patsubst modules/%,%,$(wildcard modules/*))

modules:=$(priority_modules) $(filter-out $(priority_modules),$(modules_all))

qemu:=qemu-system-x86_64 -no-reboot \
	-audiodev pa,id=speaker -machine pcspk-audiodev=speaker \
	-machine accel=kvm -cpu host \
	-vga vmware \
	-m 32M \
	-serial stdio -M smm=off --d int \
	-device ahci,id=ahci -drive file=$(iso_path),id=disk,if=none,format=raw -device ide-hd,drive=disk,bus=ide.0

needed_tools:=nasm clang lld grub-mkrescue mtools xorriso qemu-system-x86_64

all: check-tools clean build debug

check-tools:
	@for tool in $(needed_tools); do \
		if ! which $${tool} >/dev/null 2>&1; then \
			echo "Error: $${tool} not found in PATH"; \
			exit 1; \
		fi; \
	done

$(modules):
	@mkdir -p $(build_output)
	@make -C modules/$@ root=$(CURDIR) output=$(CURDIR)/$(build_output) module_makefile=$(CURDIR)/$(module_makefile)

build: $(modules)
	@mkdir -p $(dir $(iso_path))
	@grub-mkrescue -o $(iso_path) $(build_output) \
		--product-name="ENAos" \
		--compress="none" \
		--fonts="" \
		--locales="" \
		--themes="" \
		--install-modules="multiboot2 normal all_video font gfxterm \
		part_acorn part_amiga part_apple part_bsd part_dfly \
		part_dvh part_gpt part_plan part_sun part_sunpc" \
		> /dev/null 2>&1

debug:
	@echo Debugging
	@$(qemu) -no-shutdown

clean:
	@rm -rf bin
	@rm -rf $(output)

clean-modules:
	@for module in $(modules); do \
		rm -r modules/$$module/obj > /dev/null 2>&1 || true; \
	done

.PHONY: all build debug clean clean_modules $(modules)
