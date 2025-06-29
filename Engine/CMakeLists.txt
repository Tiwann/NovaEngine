set(ENGINE_MODULES Core)
include(Vendor/Dependencies.cmake)


file(GLOB_RECURSE NOVA_ENGINE_SRC Source/*.cpp Source/*.h)
file(GLOB_RECURSE NOVA_ENGINE_WINDOWS_SRC Source/Platform/Windows/*.cpp)
file(GLOB_RECURSE NOVA_ENGINE_LINUX_SRC Source/Platform/Linux/*.cpp)
file(GLOB_RECURSE NOVA_ENGINE_MACOS_SRC Source/Platform/MacOS/*.cpp)


if(WIN32)
    list(APPEND NOVA_ENGINE_PLATFORM_SRC ${NOVA_ENGINE_WINDOWS_SRC})
elseif (LINUX)
    list(APPEND NOVA_ENGINE_PLATFORM_SRC ${NOVA_ENGINE_LINUX_SRC})
elseif (DARWIN)
    list(APPEND NOVA_ENGINE_PLATFORM_SRC ${NOVA_ENGINE_MACOS_SRC})
endif ()

list(REMOVE_ITEM NOVA_ENGINE_SRC ${NOVA_ENGINE_PLATFORM_SRC})


add_library(NovaEngine STATIC)
target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_SRC} ${NOVA_ENGINE_PLATFORM_SRC})
set_target_properties(NovaEngine PROPERTIES CXX_STANDARD 20)
target_link_libraries(NovaEngine PUBLIC msdfgen-core msdfgen-ext assimp utf8cpp fmod-core fmod-studio glfw glad VulkanMemoryAllocator box2d Jolt imgui imguizmo STB Vulkan::Vulkan Vulkan::slang miniaudio ogg vorbis vorbisenc $<$<PLATFORM_ID:Windows>:d3d12 dxgi dxguid DirectX-Headers>)
target_include_directories(NovaEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_link_options(NovaEngine PRIVATE /NATVIS:${CMAKE_CURRENT_SOURCE_DIR}/NovaEngine.natvis)
target_compile_definitions(NovaEngine PRIVATE NOVA_CORE INTERFACE NOVA_CLIENT
        PRIVATE $<$<PLATFORM_ID:Windows>:UNICODE _UNICODE WIN32_LEAN_AND_MEAN>
        PUBLIC $<$<PLATFORM_ID:Windows>:NOVA_PLATFORM_WINDOWS NOVA_SYSTEM_NAME="Windows">
        PUBLIC $<$<PLATFORM_ID:Linux>:NOVA_PLATFORM_LINUX NOVA_SYSTEM_NAME="Linux">
        PUBLIC $<$<PLATFORM_ID:Darwin>:NOVA_PLATFORM_DARWIN NOVA_SYSTEM_NAME="Darwin">
        PUBLIC $<$<CONFIG:Debug>:NOVA_DEBUG>
        PUBLIC $<$<CONFIG:RelWithDebInfo>:NOVA_DEV>
        PUBLIC $<$<OR:$<CONFIG:MinSizeRel>,$<CONFIG:Release>>:NOVA_DIST>
        PUBLIC NOVA_ENGINE_ROOT_DIR=\"${CMAKE_SOURCE_DIR}\"
)


if(MSVC)
    target_compile_options(NovaEngine PRIVATE /MP)
    target_link_options(NovaEngine INTERFACE $<$<CONFIG:MinSizeRel>:/SUBSYSTEM:WINDOWS>)
endif ()

foreach (MODULE ${ENGINE_MODULES})
    set(MODULE_EXPORT_HEADER ${CMAKE_CURRENT_LIST_DIR}/ModuleExport.in.h)
    set(HEADER_FILE ${CMAKE_BINARY_DIR}/Include/${MODULE}Export.h)
    file(READ ${MODULE_EXPORT_HEADER} HEADER_CONTENTS)
    string(TOUPPER ${MODULE} MODULE_UPPER)
    string(REPLACE "@MODULE_NAME@" ${MODULE_UPPER} HEADER_CONTENTS ${HEADER_CONTENTS})
    file(WRITE ${HEADER_FILE} ${HEADER_CONTENTS})

    target_include_directories(NovaEngine PUBLIC ${CMAKE_BINARY_DIR}/Include)
    target_include_directories(NovaEngine PUBLIC ${MODULE}/Public)
    target_include_directories(NovaEngine PRIVATE ${MODULE}/Private)
    target_include_directories(NovaEngine PRIVATE ${MODULE})
endforeach ()