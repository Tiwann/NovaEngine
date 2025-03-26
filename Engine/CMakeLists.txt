set(NOVAENGINE_MODULES Audio Core Physics)

foreach(MODULE ${NOVAENGINE_MODULES})

    add_subdirectory(${MODULE})

    # Configure template export header
    set(HEADER_TEMPLATE "${CMAKE_CURRENT_LIST_DIR}/ModuleExport.in.h")
    set(HEADER_FILE "${CMAKE_BINARY_DIR}/Include/${MODULE}Export.h")
    file(READ "${HEADER_TEMPLATE}" HEADER_CONTENTS)
    string(TOUPPER "${MODULE}" MODULE_UPPER)
    string(REPLACE "@MODULE_NAME@" "${MODULE_UPPER}" HEADER_CONTENTS "${HEADER_CONTENTS}")
    file(WRITE "${HEADER_FILE}" "${HEADER_CONTENTS}")

    target_compile_definitions(NovaEngine-${MODULE}
            INTERFACE ${MODULE_UPPER}_SHARED
            INTERFACE ${MODULE_UPPER}_BUILD)

    target_include_directories(NovaEngine-${MODULE} PUBLIC "${CMAKE_BINARY_DIR}/Include")

endforeach()