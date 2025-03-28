set(FMOD_CORE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Library/FMOD-Core)
set(FMOD_STUDIO_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Library/FMOD-Studio)
set(FMOD_CORE_BIN_DIR ${FMOD_CORE_LIB_DIR}/Binaries)
set(FMOD_STUDIO_BIN_DIR ${FMOD_STUDIO_LIB_DIR}/Binaries)
set(FMOD_CORE_INCLUDE_DIR ${FMOD_CORE_LIB_DIR}/Include)
set(FMOD_STUDIO_INCLUDE_DIR ${FMOD_STUDIO_LIB_DIR}/Include)

add_library(fmod SHARED IMPORTED)
add_library(fmodstudio SHARED IMPORTED)

set_target_properties(fmod PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${FMOD_CORE_INCLUDE_DIR})
set_target_properties(fmodstudio PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${FMOD_STUDIO_INCLUDE_DIR})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set_target_properties(fmod PROPERTIES IMPORTED_LOCATION ${FMOD_CORE_BIN_DIR}/Windows/fmod.dll)
	set_target_properties(fmod PROPERTIES IMPORTED_IMPLIB ${FMOD_CORE_BIN_DIR}/Windows/fmod_vc.lib)
	set_target_properties(fmodstudio PROPERTIES IMPORTED_LOCATION ${FMOD_STUDIO_BIN_DIR}/Windows/fmodstudio.dll)
	set_target_properties(fmodstudio PROPERTIES IMPORTED_IMPLIB ${FMOD_STUDIO_BIN_DIR}/Windows/fmodstudio_vc.lib)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set_target_properties(fmod PROPERTIES IMPORTED_LOCATION ${FMOD_CORE_BIN_DIR}/Linux/libfmod.so)
	set_target_properties(fmodstudio PROPERTIES IMPORTED_LOCATION ${FMOD_STUDIO_BIN_DIR}/Linux/libfmodstudio.so)
else()
	message(SEND_ERROR "FMOD API not yet available for MacOS")
endif ()

file(GLOB_RECURSE SOURCES *.cpp *.h)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(NovaEngine-Audio SHARED ${SOURCES})

target_link_libraries(NovaEngine-Audio PRIVATE NovaEngine-Core fmod fmodstudio)
