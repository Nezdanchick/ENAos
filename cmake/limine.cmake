if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/limine/limine-bios.sys")
    set(LIMINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/limine")
elseif(EXISTS "${CMAKE_BINARY_DIR}/_deps/limine_binaries-src/limine-bios.sys")
    set(LIMINE_DIR "${CMAKE_BINARY_DIR}/_deps/limine_binaries-src")
else()
    include(FetchContent)

    FetchContent_Declare(
        limine_binaries
        GIT_REPOSITORY https://github.com/limine-bootloader/limine.git
        GIT_TAG        v8.x-binary
    )
    FetchContent_MakeAvailable(limine_binaries)

    set(LIMINE_DIR ${limine_binaries_SOURCE_DIR})
endif()

if(NOT EXISTS "${LIMINE_DIR}/limine")
    execute_process(
        COMMAND make -C "${LIMINE_DIR}" limine
    )
endif()

set(LIMINE_COMMON_BOOT "
    protocol: multiboot2
    kernel_path: boot():/boot/${KERNEL_NAME}
    module_path: boot():/boot/${INIT_NAME}
    module_string: init
    module_path: boot():/boot/${INITRAMFS_NAME}
    module_string: initramfs
")

configure_file(${CMAKE_CURRENT_LIST_DIR}/limine.conf.in ${CMAKE_BINARY_DIR}/limine.conf @ONLY)
iso_add(${CMAKE_BINARY_DIR}/limine.conf "boot/limine.conf")
