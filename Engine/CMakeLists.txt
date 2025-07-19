include(Vendor/Vendor.cmake)

file(GLOB_RECURSE NOVA_ENGINE_SOURCES Source/*.cpp Source/*.h)
file(GLOB_RECURSE NOVA_ENGINE_WINDOWS_SOURCES Source/*/Windows/*.cpp Source/*/Windows/*.h)
file(GLOB_RECURSE NOVA_ENGINE_LINUX_SOURCES Source/*/Linux/*.cpp Source/*/Linux/*.h)

if(WIN32)
    list(REMOVE_ITEM NOVA_ENGINE_SOURCES ${NOVA_ENGINE_LINUX_SOURCES})
elseif(LINUX)
    list(REMOVE_ITEM NOVA_ENGINE_SOURCES ${NOVA_ENGINE_WINDOWS_SOURCES})
endif ()


add_library(NovaEngine)
set_target_properties(NovaEngine PROPERTIES CXX_STANDARD 23)
target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_SOURCES})
target_link_options(NovaEngine PRIVATE /NATVIS:NovaEngine.natvis)
target_include_directories(NovaEngine PUBLIC Source)
target_link_libraries(NovaEngine PUBLIC Vulkan::Vulkan Vulkan::slang miniaudio PRIVATE glfw imgui)
target_compile_definitions(NovaEngine PRIVATE NOVA_CORE INTERFACE NOVA_CLIENT)

if(NOVA_ENGINE_CUSTOM_ENTRY_POINT)
target_compile_definitions(NovaEngine PUBLIC NOVA_CUSTOM_ENTRY_POINT)
endif ()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(NovaEngine PUBLIC NOVA_DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    target_compile_definitions(NovaEngine PUBLIC NOVA_DEV)
elseif (CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    target_compile_definitions(NovaEngine PUBLIC NOVA_RELEASE)
endif ()

if(BUILD_SHARED_LIBS)
    target_compile_definitions(NovaEngine PRIVATE NOVA_BUILD_SHARED)
endif ()


if(WIN32)
    target_compile_definitions(NovaEngine PUBLIC UNICODE _UNICODE NOVA_PLATFORM_WINDOWS)
elseif (LINUX)
    target_compile_definitions(NovaEngine PUBLIC NOVA_PLATFORM_LINUX)
endif ()

