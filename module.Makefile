arch=x86_64

obj=obj
global_inc=$(root)/modules/global/include/
global_obj=$(root)/modules/global/obj

cc=clang
cxx=clang++
ld=ld.lld
as=nasm

cflags=-c -I./include/ -I$(global_inc) -O3 -ffreestanding -fno-builtin -nostdlib \
-mno-red-zone -mcmodel=kernel -Wall -Wextra -fno-pic -target $(arch)-unknown-none
cxxflags=$(cflags)
ldflags=-nostdlib -no-pie
asflags=-felf64

$(obj)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo Compiling $<
	@$(cc) $(cflags) -c $< -o $@

$(obj)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	@echo Compiling $<
	@$(cxx) $(cxxflags) -c $< -o $@

$(obj)/%.o: src/%.asm
	@mkdir -p $(dir $@)
	@echo Compiling $<
	@$(as) $(asflags) $< -o $@

c_src:=$(shell find src/ -name '*.c')
c_obj:=$(patsubst src/%.c,$(obj)/%.o,$(c_src))

cpp_src:=$(shell find src/ -name '*.cpp')
cpp_obj:=$(patsubst src/%.cpp,$(obj)/%.o,$(cpp_src))

asm_src:=$(shell find src/ -name '*.asm')
asm_obj:=$(patsubst src/%.asm,$(obj)/%.o,$(asm_src))

ifneq (,$(wildcard ./linker.ld))
linker=linker.ld
else
linker=$(root)/linker_module.ld
endif

$(target): $(c_obj) $(cpp_obj) $(asm_obj)
	@mkdir -p $(dir $@)
	@echo Linking $@
	@echo $(global_objects)
	@$(ld) $(ldflags) -T $(linker) \
		$(shell find $(obj) -name '*.o') $(shell find $(global_obj) -name '*.o') -o $@