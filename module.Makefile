ARCH=x86_64

OBJ=obj

CC=clang
CXX=clang++
LD=ld.lld
AS=nasm

CFLAGS=-c -I./include/ -I$(global_inc) -O3 -ffreestanding -fno-builtin -nostdlib \
-mno-red-zone -mcmodel=kernel -Wall -Wextra -fno-pic -target $(ARCH)-unknown-none
CXXFLAGS=-c -I./include/ -I$(global_inc) -O3 -ffreestanding -fno-builtin -nostdlib \
-mno-red-zone -mcmodel=kernel -Wall -Wextra -fno-pic -target $(ARCH)-unknown-none
LDFLAGS=-nostdlib -no-pie
ASFLAGS=-felf64

$(OBJ)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ)/%.o: src/%.asm
	@mkdir -p $(dir $@)
	@echo Compiling $<
	@$(AS) $(ASFLAGS) $< -o $@

c_src:=$(shell find src/ -name '*.c')
c_obj:=$(patsubst src/%.c,$(OBJ)/%.o,$(c_src))

cpp_src:=$(shell find src/ -name '*.cpp')
cpp_obj:=$(patsubst src/%.cpp,$(OBJ)/%.o,$(cpp_src))

asm_src:=$(shell find src/ -name '*.asm')
asm_obj:=$(patsubst src/%.asm,$(OBJ)/%.o,$(asm_src))

$(target): $(c_obj) $(cpp_obj) $(asm_obj)
	@mkdir -p $(dir $@)
	@echo Linking $@
	@echo $(global_objects)
	$(LD) $(LDFLAGS) -T linker.ld \
		$(shell find $(OBJ) -name '*.o') $(shell find $(global_obj) -name '*.o') -o $@
