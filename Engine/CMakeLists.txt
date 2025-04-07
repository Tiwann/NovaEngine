set(ENGINE_MODULES Core)
include(Dependencies/Dependencies.cmake)

file(GLOB_RECURSE SOURCES Source/*.cpp Source/*.h)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(NovaEngine SHARED ${SOURCES})

#[[if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(NovaEngine PUBLIC nvrhi_d3d12 d3d12 dxgi d3dcompiler)
endif ()]]
target_link_libraries(NovaEngine PUBLIC glfw VulkanMemoryAllocator box2d Jolt imgui imguizmo stb nvrhi nvrhi_vk Vulkan::Vulkan)
target_include_directories(NovaEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_compile_definitions(NovaEngine PRIVATE NOVA_CORE INTERFACE NOVA_CLIENT)
set_target_properties(NovaEngine PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Binaries"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Binaries"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Binaries"
)

if(CMAKE_BUILD_TYPE MATCHES "DebugNoEditor" OR CMAKE_BUILD_TYPE MATCHES "DebugEditor")
    target_compile_definitions(NovaEngine PUBLIC NOVA_DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES "DevNoEditor" OR CMAKE_BUILD_TYPE MATCHES "DevEditor")
    target_compile_definitions(NovaEngine PUBLIC NOVA_DEV)
elseif(CMAKE_BUILD_TYPE MATCHES "DistNoEditor" or CMAKE_BUILD_TYPE MATCHES "Dist")
    target_compile_definitions(NovaEngine PUBLIC NOVA_DIST)
endif()

target_compile_definitions(NovaEngine
        PUBLIC $<PLATFORM_ID:Windows>:NOVA_PLATFORM_WINDOWS NOVA_SYSTEM_NAME="Windows"
        PUBLIC $<PLATFORM_ID:Unix>:NOVA_PLATFORM_LINUX NOVA_SYSTEM_NAME="Linux"
        PUBLIC $<PLATFORM_ID:Darwin>:NOVA_PLATFORM_DARWIN NOVA_SYSTEM_NAME="Darwin")

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