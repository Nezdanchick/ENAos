file(GLOB CORE_SOURCES CONFIGURE_DEPENDS src/core/*.c)

file(GLOB ARCH_SOURCES CONFIGURE_DEPENDS 
    src/arch/${ARCH}/*.c 
    src/arch/${ARCH}/*.asm
    src/arch/${ARCH}/boot/*.asm
)

add_executable(kernel ${CORE_SOURCES} ${ARCH_SOURCES})

target_compile_options(kernel PRIVATE ${KERNEL_C_FLAGS})
target_link_options(kernel PRIVATE 
    -fuse-ld=lld 
    -nostdlib 
    -static 
    -T ${CMAKE_CURRENT_SOURCE_DIR}/src/arch/${ARCH}/linker.ld
)

target_include_directories(kernel PRIVATE 
    include 
    include/core
    include/arch/${ARCH}
)

iso_add(kernel "boot/kernel.elf")
iso_add(limine.conf "boot/limine.conf")
