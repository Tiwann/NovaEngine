set(MSDFGEN_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(MSDFGEN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(msdfgen-core SHARED IMPORTED GLOBAL)
target_include_directories(msdfgen-core INTERFACE ${MSDFGEN_INCLUDE_DIR})

add_library(msdfgen-ext SHARED IMPORTED GLOBAL)
target_include_directories(msdfgen-ext INTERFACE ${MSDFGEN_INCLUDE_DIR})


if(WIN32)
    set_target_properties(msdfgen-core PROPERTIES IMPORTED_LOCATION ${MSDFGEN_BIN_DIR}/Release/Windows-x64/msdfgen-core.dll)
    set_target_properties(msdfgen-core PROPERTIES IMPORTED_IMPLIB ${MSDFGEN_BIN_DIR}/Release/Windows-x64/msdfgen-core.lib)
    target_compile_definitions(msdfgen-core INTERFACE MSDFGEN_PUBLIC)

    set_target_properties(msdfgen-ext PROPERTIES IMPORTED_LOCATION ${MSDFGEN_BIN_DIR}/Release/Windows-x64/msdfgen-ext.dll)
    set_target_properties(msdfgen-ext PROPERTIES IMPORTED_IMPLIB ${MSDFGEN_BIN_DIR}/Release/Windows-x64/msdfgen-ext.lib)
elseif (LINUX)
    message(SEND_ERROR "msdfgen not yet available for MacOS")
elseif(APPLE)
    message(SEND_ERROR "msdfgen not yet available for MacOS")
else ()
    message(SEND_ERROR "msdfgen not yet available for this platform")
endif ()
