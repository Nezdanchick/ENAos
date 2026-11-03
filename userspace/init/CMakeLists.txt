userspace_program(init)

add_custom_command(
    TARGET init POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:init> $<TARGET_FILE:init>
)

iso_add(init "boot/${INIT_NAME}")
cpio_add(init.conf "etc/init.conf")
