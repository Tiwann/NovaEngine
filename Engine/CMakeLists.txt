set(ENGINE_MODULES Core)
include(Dependencies/Dependencies.cmake)

file(GLOB_RECURSE NOVA_ENGINE_SRC Source/*.cpp Source/*.h)
if(WIN32)
    file(GLOB_RECURSE NOVA_ENGINE_PLATFORM_SRC CONFIGURE_DEPENDS
            ${MODULE_PATH}/Platform/Windows/*.cpp
            ${MODULE_PATH}/Platform/Windows/*.h
    )
elseif(UNIX AND NOT APPLE)
    file(GLOB_RECURSE NOVA_ENGINE_PLATFORM_SRC CONFIGURE_DEPENDS
            ${MODULE_PATH}/Platform/Linux/*.cpp
            ${MODULE_PATH}/Platform/Linux/*.h
    )
elseif(APPLE)
    file(GLOB_RECURSE NOVA_ENGINE_PLATFORM_SRC CONFIGURE_DEPENDS
            ${MODULE_PATH}/Platform/MacOS/*.cpp
            ${MODULE_PATH}/Platform/MacOS/*.h
    )
endif()

list(REMOVE_ITEM NOVA_ENGINE_SRC ${NOVA_ENGINE_PLATFORM_SRC})

set(CMAKE_CXX_STANDARD 20)
add_library(NovaEngine SHARED)
target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_SRC} ${NOVA_ENGINE_PLATFORM_SRC})
target_link_libraries(NovaEngine PUBLIC utf8cpp fmod-core fmod-studio glfw glad VulkanMemoryAllocator box2d Jolt imgui imguizmo stb Vulkan::Vulkan)
target_include_directories(NovaEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_compile_definitions(NovaEngine PRIVATE NOVA_CORE INTERFACE NOVA_CLIENT)
set_target_properties(NovaEngine
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Binaries"
        PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Binaries"
        PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Binaries"
)

if(CMAKE_BUILD_TYPE MATCHES "DebugNoEditor" OR CMAKE_BUILD_TYPE MATCHES "DebugEditor")
    target_compile_definitions(NovaEngine PUBLIC NOVA_DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES "DevNoEditor" OR CMAKE_BUILD_TYPE MATCHES "DevEditor")
    target_compile_definitions(NovaEngine PUBLIC NOVA_DEV)
elseif(CMAKE_BUILD_TYPE MATCHES "DistNoEditor" OR CMAKE_BUILD_TYPE MATCHES "Dist")
    target_compile_definitions(NovaEngine PUBLIC NOVA_DIST)
endif()

target_compile_definitions(NovaEngine
        PUBLIC $<$<PLATFORM_ID:Windows>:NOVA_PLATFORM_WINDOWS NOVA_SYSTEM_NAME="Windows">
        PUBLIC $<$<PLATFORM_ID:Unix>:NOVA_PLATFORM_LINUX NOVA_SYSTEM_NAME="Linux">
        PUBLIC $<$<PLATFORM_ID:Darwin>:NOVA_PLATFORM_DARWIN NOVA_SYSTEM_NAME="Darwin">)

foreach (MODULE ${ENGINE_MODULES})
    set(MODULE_EXPORT_HEADER ${CMAKE_CURRENT_LIST_DIR}/ModuleExport.in.h)
    set(HEADER_FILE ${CMAKE_BINARY_DIR}/Include/${MODULE}Export.h)
    file(READ ${MODULE_EXPORT_HEADER} HEADER_CONTENTS)
    string(TOUPPER ${MODULE} MODULE_UPPER)
    string(REPLACE "@MODULE_NAME@" ${MODULE_UPPER} HEADER_CONTENTS ${HEADER_CONTENTS})
    file(WRITE ${HEADER_FILE} ${HEADER_CONTENTS})

    target_compile_definitions(NovaEngine
            INTERFACE MODULE_SHARED
            INTERFACE MODULE_BUILD)

    target_include_directories(NovaEngine PUBLIC ${CMAKE_BINARY_DIR}/Include)
    target_include_directories(NovaEngine PUBLIC ${MODULE}/Public)
    target_include_directories(NovaEngine PRIVATE ${MODULE}/Private)
    target_include_directories(NovaEngine PRIVATE ${MODULE})
endforeach ()