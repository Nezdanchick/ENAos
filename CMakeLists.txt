cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
endif()

project(ENAos LANGUAGES C ASM_NASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(cmake/arch.cmake)
include(cmake/iso.cmake)
include(cmake/cpio.cmake)
include(cmake/limine.cmake)

add_subdirectory(kernel)

file(GLOB USERSPACE_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/userspace ${CMAKE_CURRENT_SOURCE_DIR}/userspace/*)

foreach(DIR ${USERSPACE_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/userspace/${DIR} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/userspace/${DIR}/CMakeLists.txt)
        add_subdirectory(userspace/${DIR})
    endif()
endforeach()

set(ISO_ROOT "${CMAKE_BINARY_DIR}/iso_root")
set(ISO_IMAGE "${CMAKE_BINARY_DIR}/ENAos.iso")
set(CPIO_ARCHIVE "${ISO_ROOT}/boot/initramfs.cpio")

cpio_build("${CPIO_ARCHIVE}")
iso_build("${ISO_IMAGE}")

add_custom_target(run
    DEPENDS iso
    COMMAND qemu-system-x86_64
            -cdrom ${ISO_IMAGE}
            -serial stdio
            -m 256M
            -no-reboot
            -no-shutdown
)
