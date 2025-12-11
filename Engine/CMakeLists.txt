include(Vendor/Vendor.cmake)

file(GLOB_RECURSE NOVA_ENGINE_SOURCES Source/*.cpp Source/*.hpp Source/*.c Source/*.h Source/*.cc Source/*.hh Source/*.hxx)
file(GLOB_RECURSE NOVA_ENGINE_WINDOWS_SOURCES Source/*/Windows/*.cpp Source/*/Windows/*.h)
file(GLOB_RECURSE NOVA_ENGINE_LINUX_SOURCES Source/*/Linux/*.cpp Source/*/Linux/*.h)

if(WIN32)
    list(REMOVE_ITEM NOVA_ENGINE_SOURCES ${NOVA_ENGINE_LINUX_SOURCES})
elseif(LINUX)
    list(REMOVE_ITEM NOVA_ENGINE_SOURCES ${NOVA_ENGINE_WINDOWS_SOURCES})
endif ()

set(NOVA_ENGINE_D3D12_SOURCES
        Source/Rendering/D3D12/Device.h
        Source/Rendering/D3D12/Device.cpp
        Source/Rendering/D3D12/Swapchain.h
        Source/Rendering/D3D12/Swapchain.cpp
        Source/Rendering/D3D12/Conversions.h
        Source/Rendering/D3D12/Surface.h
        Source/Rendering/D3D12/Surface.cpp
        Source/Rendering/D3D12/Queue.h
        Source/Rendering/D3D12/Queue.cpp
        Source/Rendering/D3D12/Fence.h
        Source/Rendering/D3D12/Fence.cpp
        Source/Rendering/D3D12/CommandBuffer.h
        Source/Rendering/D3D12/CommandBuffer.cpp
        Source/Rendering/D3D12/CommandPool.h
        Source/Rendering/D3D12/CommandPool.cpp
        Source/Rendering/D3D12/Buffer.h
        Source/Rendering/D3D12/Buffer.cpp
)

set(NOVA_ENGINE_NULL_SOURCES
    Source/Rendering/Null/Device.h
    Source/Rendering/Null/Device.cpp)

add_library(NovaEngine STATIC)
set_target_properties(NovaEngine PROPERTIES CXX_STANDARD 23)
target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_SOURCES})
target_link_options(NovaEngine PRIVATE /NATVIS:NovaEngine.natvis)
target_include_directories(NovaEngine PUBLIC Source PRIVATE Source/External)
target_link_libraries(NovaEngine PUBLIC vulkan slang miniaudio vorbisfile vorbis vorbisenc imgui box2d glfw PRIVATE assimp::assimp spirv-reflect-static kissfft::kissfft)
target_compile_definitions(NovaEngine PRIVATE NOVA_CORE INTERFACE NOVA_CLIENT PUBLIC NOVA_ENGINE_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

if(WIN32)
    target_link_libraries(NovaEngine PUBLIC kernel32)
    target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_WINDOWS_SOURCES})
    if(NOVA_ENGINE_BUILD_D3D12)
        target_sources(NovaEngine PRIVATE ${NOVA_ENGINE_D3D12_SOURCES})
        target_compile_definitions(NovaEngine PUBLIC NOVA_HAS_D3D12)
        target_link_libraries(NovaEngine PRIVATE Microsoft::DirectX-Headers d3d12 dxgi PUBLIC D3D12MemoryAllocator)
    endif ()
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