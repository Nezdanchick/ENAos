cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
endif()

include(cmake/version.cmake)

project(${OS_NAME} LANGUAGES C ASM_NASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_definitions(
    OS_NAME="${OS_NAME}"
    OS_VERSION="${OS_VERSION}"
)

include(cmake/arch.cmake)
include(cmake/iso.cmake)
include(cmake/cpio.cmake)
include(cmake/limine.cmake)
include(cmake/userspace.cmake)

add_subdirectory(kernel)

file(GLOB USERSPACE_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/userspace ${CMAKE_CURRENT_SOURCE_DIR}/userspace/*)

foreach(DIR ${USERSPACE_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/userspace/${DIR} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/userspace/${DIR}/CMakeLists.txt)
        add_subdirectory(userspace/${DIR})
    endif()
endforeach()

set(ISO_IMAGE "${CMAKE_BINARY_DIR}/${OS_NAME}.iso")
set(CPIO_ARCHIVE "${CMAKE_BINARY_DIR}/${INITRAMFS_NAME}")

cpio_build("${CPIO_ARCHIVE}")
iso_add("${CPIO_ARCHIVE}" "boot/${INITRAMFS_NAME}")
iso_build("${ISO_IMAGE}")

add_custom_target(run
    DEPENDS iso
    COMMAND ${QEMU_BIN}
            -cdrom ${ISO_IMAGE}
            -chardev stdio,id=char0,logfile=${CMAKE_BINARY_DIR}/qemu.log
            -serial chardev:char0
            -m 256M
            -no-reboot
            -no-shutdown
)
