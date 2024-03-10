ARCH=x86_64
RUNNER=qemu-system-$(ARCH)

KERNEL=./bin/$(ARCH)-kernel
ISO=./bin/nandos.iso
ISO_PATH=target/$(ARCH)/iso

x86_64_asm_source_files := $(shell find src/x86_64 -name *.asm)
x86_64_asm_object_files := $(patsubst src/x86_64/%.asm, bin/x86_64/%.o, $(x86_64_asm_source_files))

x86_64_c_source_files := $(shell find src/x86_64 -name *.c)
x86_64_c_object_files := $(patsubst src/x86_64/%.c, bin/x86_64/%.o, $(x86_64_c_source_files))

kernel_c_source_files := $(shell find src/kernel -name *.c)
kernel_c_object_files := $(patsubst src/kernel/%.c, bin/kernel/%.o, $(kernel_c_source_files))

CC_FLAGS=-c -I./src/include/ -ffreestanding -O2 -Wall -Wextra
LD_FLAGS=-n -o $(KERNEL) -T ./target/$(ARCH)/linker.ld

	
all: build create-iso debug

build: $(kernel_c_object_files) $(x86_64_c_object_files) $(x86_64_asm_object_files)
	@echo Building...
	@mkdir -p $(dir $(KERNEL))
	ld $(LD_FLAGS) $(kernel_c_object_files) $(x86_64_c_object_files) $(x86_64_asm_object_files)

debug:
	@echo Debugging...
	$(RUNNER) $(ISO) --no-reboot
	
create-iso: build
	@mkdir -p $(dir $(ISO))
	cp $(KERNEL) $(ISO_PATH)/boot/kernel
	grub-mkrescue \
	--product-name="nandos" \
	--compress="no" \
	--fonts="unicode" \
	--locales="" \
	--themes="" \
	--install-modules="multiboot2 \
	part_acorn part_amiga part_apple part_bsd part_dfly \
	part_dvh part_gpt part_plan part_sun part_sunpc" \
	-o $(ISO) $(ISO_PATH)

clean:
	rm -rf ./bin
	
$(x86_64_asm_object_files): bin/x86_64/%.o : src/x86_64/%.asm
	mkdir -p $(dir $@)
	nasm -felf64 $(patsubst bin/x86_64/%.o, src/x86_64/%.asm, $@) -o $@

$(x86_64_c_object_files): bin/x86_64/%.o : src/x86_64/%.c
	mkdir -p $(dir $@)
	clang $(CC_FLAGS) $(patsubst bin/x86_64/%.o, src/x86_64/%.c, $@) -o $@

$(kernel_c_object_files): bin/kernel/%.o : src/kernel/%.c
	mkdir -p $(dir $@)
	clang $(CC_FLAGS) $(patsubst bin/kernel/%.o, src/kernel/%.c, $@) -o $@

.PHONY: all build debug create-iso clean
