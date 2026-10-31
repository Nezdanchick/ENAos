set(ARCH "x86_64")

if(ARCH STREQUAL "x86_64")
    set(TARGET_TRIPLE "x86_64-unknown-none-elf")
    set(QEMU_BIN "qemu-system-x86_64")
else()
    message(FATAL_ERROR "Unknown arch: ${ARCH}")
endif()

set(KERNEL_C_FLAGS
    $<$<COMPILE_LANGUAGE:C>:
        -target ${TARGET_TRIPLE}
        -ffreestanding
        -fno-builtin
        -fno-stack-protector
        -fno-pic
        -fno-pie
        -mno-red-zone
        -mcmodel=kernel
        -Wall
        -Wextra
        -O2
    >
)

set(USER_C_FLAGS
    $<$<COMPILE_LANGUAGE:C>:
        -target ${TARGET_TRIPLE}
        -ffreestanding
        -fno-builtin
        -fno-stack-protector
        -fno-pic
        -fno-pie
        -mno-red-zone
        -mcmodel=small
        -Wall
        -Wextra
        -O2
    >
)

set(USER_LINK_FLAGS
    -fuse-ld=lld
    -nostdlib
    -static
)
