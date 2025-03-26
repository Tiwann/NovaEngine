set(MODULE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(FMOD_CORE_LIB_DIR "${MODULE_DIR}/Library/FMOD Core")
set(FMOD_STUDIO_LIB_DIR "${MODULE_DIR}/Library/FMOD Studio")
set(FMOD_CORE_BIN_DIR "${FMOD_CORE_LIB_DIR}/Binaries")
set(FMOD_STUDIO_BIN_DIR "${FMOD_CORE_LIB_DIR}io/Binaries")
file(GLOB_RECURSE SOURCES ${MODULE_DIR}/Source/*.cpp ${MODULE_DIR}/Source/*.h)

include(FetchContent)
FetchContent_Declare(
	box2d
	GIT_REPOSITORY https://github.com/erincatto/box2d.git
	GIT_TAG v3.0.0
)
FetchContent_MakeAvailable(box2d)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(NovaEngine-Audio SHARED ${SOURCES})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_link_libraries(NovaEngine-Audio PRIVATE ${FMOD_CORE_BIN_DIR}/Windows/fmodL.dll)
		target_link_libraries(NovaEngine-Audio PRIVATE ${FMOD_STUDIO_BIN_DIR}/Windows/fmodstudioL.dll)
	elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
		target_link_libraries(NovaEngine-Audio PRIVATE ${FMOD_CORE_BIN_DIR}/Windows/fmod.dll)
		target_link_libraries(NovaEngine-Audio PRIVATE ${FMOD_STUDIO_BIN_DIR}/Windows/fmodstudio.dll)
	endif ()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_link_libraries(NovaEngine-Audio PRIVATE ${FMOD_CORE_BIN_DIR}/Linux/libfmodL.so)
		target_link_libraries(NovaEngine-Audio PRIVATE ${FMOD_STUDIO_BIN_DIR}/Linux/libfmodstudioL.so)
	elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
		target_link_libraries(NovaEngine-Audio PRIVATE ${FMOD_CORE_BIN_DIR}/Linux/libfmod.so)
		target_link_libraries(NovaEngine-Audio PRIVATE ${FMOD_STUDIO_BIN_DIR}/Linux/libfmodstudio.so)
	endif ()
endif ()

target_include_directories(NovaEngine-Audio PRIVATE ${FMOD_CORE_LIB_DIR}/Include)
target_link_libraries(NovaEngine-Audio PRIVATE NovaEngine-Core)
