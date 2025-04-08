set(FMOD_STUDIO_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Binaries)
set(FMOD_STUDIO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Include)

add_library(fmod-studio SHARED IMPORTED GLOBAL)

target_include_directories(fmod-studio INTERFACE ${FMOD_STUDIO_INCLUDE_DIR})

if(WIN32)
    set_target_properties(fmod-studio PROPERTIES IMPORTED_LOCATION ${FMOD_STUDIO_BIN_DIR}/Windows/fmodstudio.dll)
    set_target_properties(fmod-studio PROPERTIES IMPORTED_IMPLIB ${FMOD_STUDIO_BIN_DIR}/Windows/fmodstudio_vc.lib)
elseif (UNIX AND NOT APPLE)
    set_target_properties(fmod-studio PROPERTIES IMPORTED_LOCATION ${FMOD_STUDIO_BIN_DIR}/Linux/libfmodstudio.so)
elseif(APPLE)
    message(SEND_ERROR "FMOD Studio not yet available for MacOS")
else ()
    message(SEND_ERROR "FMOD Studio not yet available for this platform")
endif ()