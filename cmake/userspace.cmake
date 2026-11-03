function(userspace_program NAME)
    file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.c src/*.asm)
    add_executable(${NAME} ${SOURCES})
    target_compile_options(${NAME} PRIVATE ${USER_C_FLAGS})

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")
        set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")
    else()
        set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/userspace/linker.ld")
    endif()

    target_link_options(${NAME} PRIVATE ${USER_LINK_FLAGS} -T ${LINKER_SCRIPT})

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include")
        target_include_directories(${NAME} PRIVATE include)
    endif()

    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME "${NAME}.elf")
endfunction()
