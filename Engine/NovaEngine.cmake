set(NOVAENGINE_MODULES Audio Core Physics)

foreach(MODULE ${NOVAENGINE_MODULES})
    include(${CMAKE_CURRENT_LIST_DIR}/${MODULE}/NovaEngine-${MODULE}.cmake)

    # Configure template export header
    set(HEADER_TEMPLATE "${CMAKE_CURRENT_LIST_DIR}/ModuleExport.in.h")
    set(HEADER_FILE "${CMAKE_BINARY_DIR}/Include/${MODULE}Export.h")
    file(READ "${HEADER_TEMPLATE}" HEADER_CONTENTS)
    string(TOUPPER "${MODULE}" MODULE_UPPER)
    string(REPLACE "@MODULE_NAME@" "${MODULE_UPPER}" HEADER_CONTENTS "${HEADER_CONTENTS}")
    file(WRITE "${HEADER_FILE}" "${HEADER_CONTENTS}")

    target_include_directories(NovaEngine-${MODULE} PRIVATE "${CMAKE_BINARY_DIR}/Include")
endforeach()