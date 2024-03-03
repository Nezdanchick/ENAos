ARCH=x86_64
RUNNER=qemu-system-$(ARCH)

KERNEL=./bin/kernel
ISO=./bin/nandos.iso

x86_64_asm_source_files := $(shell find src/x86_64 -name *.asm)
x86_64_asm_object_files := $(patsubst src/x86_64/%.asm, bin/x86_64/%.o, $(x86_64_asm_source_files))
	
all: build create-iso debug

build: $(x86_64_asm_object_files)
	@echo Building...
	@mkdir -p $(dir $(KERNEL))
	ld -n -o $(KERNEL) -T ./target/$(ARCH)/linker.ld $(x86_64_asm_object_files)

debug:
	@echo Debugging...
	$(RUNNER) $(ISO)
	
create-iso: build
	@mkdir -p $(dir $(ISO))
	cp $(KERNEL) target/$(ARCH)/iso/system/kernel
	grub-mkrescue \
	--product-name="nandos" \
	--compress=gz \
	--fonts="" \
	--locales="" \
	--themes="" \
	--install-modules="multiboot2" \
	-o $(ISO) target/$(ARCH)/iso

clean:
	rm -rf ./bin
	
$(x86_64_asm_object_files): bin/x86_64/%.o : src/x86_64/%.asm
	mkdir -p $(dir $@)
	nasm -felf64 $(patsubst bin/x86_64/%.o, src/x86_64/%.asm, $@) -o $@

.PHONY: all build debug create-iso clean
