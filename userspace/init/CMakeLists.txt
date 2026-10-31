file(GLOB_RECURSE INIT_SOURCES
    src/*.c
    src/*.asm
)

add_executable(init ${INIT_SOURCES})

target_compile_options(init PRIVATE ${USER_C_FLAGS})
target_link_options(init PRIVATE 
    ${USER_LINK_FLAGS}
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

target_include_directories(init PRIVATE include)
set_target_properties(init PROPERTIES OUTPUT_NAME "init.elf")

add_custom_command(
    TARGET init POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:init> ${CMAKE_CURRENT_BINARY_DIR}/init.bin
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ISO_ROOT}/boot"
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/init.bin "${ISO_ROOT}/boot/init.bin"
)

cpio_add(init.conf "etc/init.conf")
