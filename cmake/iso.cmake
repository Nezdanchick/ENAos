set(ISO_ROOT "${CMAKE_BINARY_DIR}/iso_root")
set_property(GLOBAL PROPERTY ENA_ISO_TARGETS "")
set_property(GLOBAL PROPERTY ENA_ISO_COPY_COMMANDS "")
set_property(GLOBAL PROPERTY ENA_ISO_DEPENDS "")

function(iso_add ITEM DEST_PATH)
    set(DEST_FULL "${ISO_ROOT}/${DEST_PATH}")
    get_filename_component(DEST_DIR "${DEST_FULL}" DIRECTORY)

    if(TARGET ${ITEM})
        set_property(GLOBAL APPEND PROPERTY ENA_ISO_TARGETS "${ITEM}")
        set(SRC_FILE "$<TARGET_FILE:${ITEM}>")
    else()
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ITEM}")
            set(SRC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${ITEM}")
        else()
            set(SRC_FILE "${ITEM}")
        endif()
        set_property(GLOBAL APPEND PROPERTY ENA_ISO_DEPENDS "${SRC_FILE}")
    endif()

    set_property(GLOBAL APPEND PROPERTY ENA_ISO_COPY_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${SRC_FILE}" "${DEST_FULL}"
    )
endfunction()

function(iso_build OUTPUT_IMAGE)
    get_property(ISO_TARGETS GLOBAL PROPERTY ENA_ISO_TARGETS)
    get_property(ISO_DEPENDS GLOBAL PROPERTY ENA_ISO_DEPENDS)
    get_property(ISO_COPY_COMMANDS GLOBAL PROPERTY ENA_ISO_COPY_COMMANDS)

    add_custom_target(iso
        DEPENDS ${ISO_TARGETS} ${ISO_DEPENDS}
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
