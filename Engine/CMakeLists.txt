set(ENGINE_MODULES Core)
include(Dependencies/Dependencies.cmake)
include(../AutoCopy.cmake)


file(GLOB_RECURSE NOVA_ENGINE_SRC Source/*.cpp Source/*.h)
file(GLOB_RECURSE NOVA_ENGINE_PLATFORM_SRC CONFIGURE_DEPENDS
        $<$<PLATFORM_ID:Windows>:${MODULE_PATH}/Platform/Windows/*.cpp ${MODULE_PATH}/Platform/Windows/*.h>
        $<$<PLATFORM_ID:Linux>:${MODULE_PATH}/Platform/Linux/*.cpp ${MODULE_PATH}/Platform/Linux/*.h>
        $<$<PLATFORM_ID:Darwin>:${MODULE_PATH}/Platform/MacOS/*.cpp ${MODULE_PATH}/Platform/MacOS/*.h>
)

list(REMOVE_ITEM NOVA_ENGINE_SRC ${NOVA_ENGINE_PLATFORM_SRC})

set(CMAKE_CXX_STANDARD 20)
add_library(NovaEngine STATIC)
target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_SRC} ${NOVA_ENGINE_PLATFORM_SRC})
target_link_libraries(NovaEngine PUBLIC slang assimp utf8cpp fmod-core fmod-studio glfw glad VulkanMemoryAllocator box2d Jolt ImGui imguizmo STB Vulkan::Vulkan miniaudio ogg vorbis vorbisenc tinygltf $<$<PLATFORM_ID:Windows>:d3d12 dxgi dxguid DirectX-Headers>)
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
    add_compile_options(/MP)
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