include(Vendor/Vendor.cmake)

file(GLOB_RECURSE NOVA_ENGINE_SOURCES Source/*.cpp Source/*.hpp Source/*.c Source/*.h Source/*.cc Source/*.hh Source/*.hxx)
file(GLOB_RECURSE NOVA_ENGINE_WINDOWS_SOURCES Source/*/Windows/*.cpp Source/*/Windows/*.h)
file(GLOB_RECURSE NOVA_ENGINE_LINUX_SOURCES Source/*/Linux/*.cpp Source/*/Linux/*.h)

if(WIN32)
    list(REMOVE_ITEM NOVA_ENGINE_SOURCES ${NOVA_ENGINE_LINUX_SOURCES})
elseif(LINUX)
    list(REMOVE_ITEM NOVA_ENGINE_SOURCES ${NOVA_ENGINE_WINDOWS_SOURCES})
endif ()


add_library(NovaEngine STATIC)
set_target_properties(NovaEngine PROPERTIES CXX_STANDARD 23)
target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_SOURCES})
target_link_options(NovaEngine PRIVATE /NATVIS:NovaEngine.natvis)
target_include_directories(NovaEngine PUBLIC Source)
target_link_libraries(NovaEngine PUBLIC vulkan slang miniaudio imgui box2d glfw PRIVATE assimp::assimp)
target_compile_definitions(NovaEngine PRIVATE NOVA_CORE INTERFACE NOVA_CLIENT PUBLIC NOVA_ENGINE_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

if(WIN32)
    target_link_libraries(NovaEngine PUBLIC kernel32)
    target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_WINDOWS_SOURCES})
endif ()

if(NOVA_ENGINE_CUSTOM_ENTRY_POINT)
    target_compile_definitions(NovaEngine PUBLIC NOVA_CUSTOM_ENTRY_POINT)
endif ()

if(NOVA_ENGINE_INCLUDE_AUDIO)
    target_compile_definitions(NovaEngine PUBLIC NOVA_HAS_AUDIO)
endif ()

if(NOVA_ENGINE_INCLUDE_PHYSICS)
    target_compile_definitions(NovaEngine PUBLIC NOVA_HAS_PHYSICS)
endif ()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(NovaEngine PUBLIC NOVA_DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    target_compile_definitions(NovaEngine PUBLIC NOVA_DEV)
elseif (CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    target_compile_definitions(NovaEngine PUBLIC NOVA_RELEASE)
endif ()

if(WIN32)
    target_compile_definitions(NovaEngine PUBLIC UNICODE _UNICODE NOVA_PLATFORM_WINDOWS _CRT_SECURE_NO_WARNINGS)
elseif (LINUX)
    target_compile_definitions(NovaEngine PUBLIC NOVA_PLATFORM_LINUX)
endif ()

