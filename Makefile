iso_path=bin/ENAos.iso

build_output:=iso
module_makefile:=module.Makefile

priority_modules:=test global

modules_all:=$(patsubst modules/%,%,$(wildcard modules/*))

modules:=$(priority_modules) $(filter-out $(priority_modules),$(modules_all))

KVM_FLAGS := $(shell [ -r /dev/kvm ] && echo "-machine accel=kvm -cpu host" || echo "-machine accel=tcg")

qemu:=qemu-system-x86_64 -no-reboot \
    -audiodev pa,id=speaker -machine pcspk-audiodev=speaker \
    $(KVM_FLAGS) \
    -vga vmware \
    -m 32M \
    -serial stdio -M smm=off --d int \
    -usb -device usb-ehci,id=ehci \
    -drive if=none,id=usbstick,file=$(iso_path),format=raw \
    -device usb-storage,drive=usbstick

needed_tools:=nasm clang lld mtools xorriso qemu-system-x86_64 wget

all: check-tools clean build debug

check-tools:
	@for tool in $(needed_tools); do \
		if ! which $${tool} >/dev/null 2>&1; then \
			echo "Error: $${tool} not found in PATH"; \
			exit 1; \
		fi; \
	done

limine:
	@if [ ! -d limine ]; then \
		git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1; \
		make -C limine; \
	fi

$(modules):
	@mkdir -p $(build_output)
	@make -C modules/$@ root=$(CURDIR) output=$(CURDIR)/$(build_output) module_makefile=$(CURDIR)/$(module_makefile)

build: $(modules) limine
	@mkdir -p $(dir $(iso_path))
	@mkdir -p $(build_output)/boot/limine
	@cp limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin $(build_output)/boot/limine/
	@xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		$(build_output) -o $(iso_path) > /dev/null 2>&1
	@./limine/limine bios-install $(iso_path) > /dev/null 2>&1


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
