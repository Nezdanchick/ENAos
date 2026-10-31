set(ISO_ROOT "${CMAKE_BINARY_DIR}/iso_root")
set_property(GLOBAL PROPERTY ENA_ISO_TARGETS "")
set_property(GLOBAL PROPERTY ENA_ISO_COPY_COMMANDS "")
set_property(GLOBAL PROPERTY ENA_ISO_DEPENDS "")

function(iso_add ITEM DEST_PATH)
    set(DEST_FULL "${ISO_ROOT}/${DEST_PATH}")
    get_filename_component(DEST_DIR "${DEST_FULL}" DIRECTORY)

    if(TARGET ${ITEM})
        add_custom_command(
            TARGET ${ITEM} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${ITEM}> "${DEST_FULL}"
        )
        set_property(GLOBAL APPEND PROPERTY ENA_ISO_TARGETS "${ITEM}")
    else()
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ITEM}")
            set(SRC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${ITEM}")
        elseif(EXISTS "${ITEM}")
            set(SRC_FILE "${ITEM}")
        else()
            message(FATAL_ERROR "iso_add: File '${ITEM}' not found in ${CMAKE_CURRENT_SOURCE_DIR}")
        endif()

        set_property(GLOBAL APPEND PROPERTY ENA_ISO_DEPENDS "${SRC_FILE}")
        set_property(GLOBAL APPEND PROPERTY ENA_ISO_COPY_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy "${SRC_FILE}" "${DEST_FULL}"
        )
    endif()
endfunction()

function(iso_build OUTPUT_IMAGE)
    get_property(ISO_TARGETS GLOBAL PROPERTY ENA_ISO_TARGETS)
    get_property(ISO_DEPENDS GLOBAL PROPERTY ENA_ISO_DEPENDS)
    get_property(ISO_COPY_COMMANDS GLOBAL PROPERTY ENA_ISO_COPY_COMMANDS)

    add_custom_target(iso
        DEPENDS ${ISO_TARGETS} ${ISO_DEPENDS} init initramfs
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ISO_ROOT}/boot/limine
        ${ISO_COPY_COMMANDS}
        
        COMMAND ${CMAKE_COMMAND} -E copy ${LIMINE_DIR}/limine-bios.sys ${ISO_ROOT}/boot/limine/
        COMMAND ${CMAKE_COMMAND} -E copy ${LIMINE_DIR}/limine-bios-cd.bin ${ISO_ROOT}/boot/limine/
        COMMAND ${CMAKE_COMMAND} -E copy ${LIMINE_DIR}/limine-uefi-cd.bin ${ISO_ROOT}/boot/limine/

        COMMAND xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin
                -no-emul-boot -boot-load-size 4 -boot-info-table
                --efi-boot boot/limine/limine-uefi-cd.bin
                -efi-boot-part --efi-boot-image --protective-msdos-label
                ${ISO_ROOT} -o ${OUTPUT_IMAGE}

        COMMAND ${LIMINE_DIR}/limine bios-install ${OUTPUT_IMAGE}
    )
endfunction()
