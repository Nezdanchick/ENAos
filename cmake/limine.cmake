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
