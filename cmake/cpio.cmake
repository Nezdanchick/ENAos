set(CPIO_ROOT "${CMAKE_BINARY_DIR}/initramfs_root")
set_property(GLOBAL PROPERTY ENA_CPIO_TARGETS "")
set_property(GLOBAL PROPERTY ENA_CPIO_COPY_COMMANDS "")
set_property(GLOBAL PROPERTY ENA_CPIO_DEPENDS "")

function(cpio_add ITEM DEST_PATH)
    set(DEST_FULL "${CPIO_ROOT}/${DEST_PATH}")
    get_filename_component(DEST_DIR "${DEST_FULL}" DIRECTORY)

    if(TARGET ${ITEM})
        add_custom_command(
            TARGET ${ITEM} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${ITEM}> "${DEST_FULL}"
        )
        set_property(GLOBAL APPEND PROPERTY ENA_CPIO_TARGETS "${ITEM}")
    else()
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ITEM}")
            set(SRC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${ITEM}")
        elseif(EXISTS "${ITEM}")
            set(SRC_FILE "${ITEM}")
        else()
            message(FATAL_ERROR "cpio_add: File '${ITEM}' not found in ${CMAKE_CURRENT_SOURCE_DIR}")
        endif()

        set_property(GLOBAL APPEND PROPERTY ENA_CPIO_DEPENDS "${SRC_FILE}")
        set_property(GLOBAL APPEND PROPERTY ENA_CPIO_COPY_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy "${SRC_FILE}" "${DEST_FULL}"
        )
    endif()
endfunction()

function(cpio_build OUTPUT_ARCHIVE)
    get_property(CPIO_TARGETS GLOBAL PROPERTY ENA_CPIO_TARGETS)
    get_property(CPIO_DEPENDS GLOBAL PROPERTY ENA_CPIO_DEPENDS)
    get_property(CPIO_COPY_COMMANDS GLOBAL PROPERTY ENA_CPIO_COPY_COMMANDS)
    get_filename_component(ARCHIVE_DIR "${OUTPUT_ARCHIVE}" DIRECTORY)

    set(CPIO_SCRIPT "${CMAKE_BINARY_DIR}/make_cpio.sh")
    file(WRITE "${CPIO_SCRIPT}" "#!/bin/sh\nset -e\ncd \"$1\"\nfind . -mindepth 1 -print0 | cpio --null -o --format=newc > \"$2\"\n")

    add_custom_command(
        OUTPUT "${OUTPUT_ARCHIVE}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CPIO_ROOT}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ARCHIVE_DIR}"
        ${CPIO_COPY_COMMANDS}
        COMMAND /bin/sh "${CPIO_SCRIPT}" "${CPIO_ROOT}" "${OUTPUT_ARCHIVE}"
        DEPENDS ${CPIO_TARGETS} ${CPIO_DEPENDS}
    )

    add_custom_target(initramfs ALL DEPENDS "${OUTPUT_ARCHIVE}")
endfunction()
