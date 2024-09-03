ARCH:=x86_64
RUNNER:=qemu-system-$(ARCH) -no-reboot -no-shutdown \
-audiodev pa,id=speaker -machine pcspk-audiodev=speaker \
-machine accel=kvm -cpu host \
-vga qxl \
-serial stdio -M smm=off --d int

KERNEL=./bin/$(ARCH)-kernel
ISO=./bin/nandos.iso
ISO_PATH=target/$(ARCH)/iso

x86_64_asm_source_files := $(shell find src/x86_64 -name *.asm)
x86_64_asm_object_files := $(patsubst src/x86_64/%.asm, bin/x86_64/%.o, $(x86_64_asm_source_files))

x86_64_c_source_files := $(shell find src/x86_64 -name *.c)
x86_64_c_object_files := $(patsubst src/x86_64/%.c, bin/x86_64/%.o, $(x86_64_c_source_files))

kernel_c_source_files := $(shell find src/kernel -name *.c)
kernel_c_object_files := $(patsubst src/kernel/%.c, bin/kernel/%.o, $(kernel_c_source_files))

CC=clang
LD=ld.lld
AS=nasm

CC_FLAGS=-c -I./src/include/ -O3 -ffreestanding -fno-builtin -nostdlib \
-mno-red-zone -mcmodel=kernel -Wall -Wextra -fno-pic -target $(ARCH)-unknown-none
LD_FLAGS=-o $(KERNEL) -T ./target/$(ARCH)/linker.ld -nostdlib -no-pie
AS_FLAGS=-felf64

all: clean build create-iso debug

build: $(kernel_c_object_files) $(x86_64_c_object_files) $(x86_64_asm_object_files)
	@echo Building...
	@mkdir -p $(dir $(KERNEL))
	$(LD) $(LD_FLAGS) $(kernel_c_object_files) $(x86_64_c_object_files) $(x86_64_asm_object_files)
	
create-iso: build
	grub-file --is-x86-multiboot2 $(KERNEL)
	@mkdir -p $(dir $(ISO))
	cp $(KERNEL) $(ISO_PATH)/boot/kernel
	grub-mkrescue -o $(ISO) $(ISO_PATH) \
	--product-name="nandos" \
	--compress="no" \
	--fonts="unicode" \
	--locales="" \
	--themes="" \
	--install-modules="multiboot2 normal all_video font gfxterm \
	part_acorn part_amiga part_apple part_bsd part_dfly \
	part_dvh part_gpt part_plan part_sun part_sunpc" \

clean:
	rm -rf ./bin

.ONESHELL:
dd: create-iso
	clear
	read -p "Enter Device Name (for example 'sda'): " device
	device_path=/dev/$$device
	echo Your device is $$device_path.
	read -p "Are you sure? [y/N]: " answer
	if [[ ($$answer = "yes" || $$answer = "y") && -e $$device_path ]]; then
		echo $$device_path is correct;
		echo dd if=$(ISO) of=$$device_path;
		sudo dd if=$(ISO) of=$$device_path;
	else
		echo $$device_path is incorrect device path;
	fi

debug:
	@echo Debugging...
	$(RUNNER) $(ISO)
	
$($(ARCH)_asm_object_files): bin/$(ARCH)/%.o : src/$(ARCH)/%.asm
	mkdir -p $(dir $@)
	$(AS) $(AS_FLAGS) $(patsubst bin/$(ARCH)/%.o, src/$(ARCH)/%.asm, $@) -o $@

$($(ARCH)_c_object_files): bin/$(ARCH)/%.o : src/$(ARCH)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) $(patsubst bin/$(ARCH)/%.o, src/$(ARCH)/%.c, $@) -o $@

$(kernel_c_object_files): bin/kernel/%.o : src/kernel/%.c
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) $(patsubst bin/kernel/%.o, src/kernel/%.c, $@) -o $@

.PHONY: all build create-iso clean dd debug
