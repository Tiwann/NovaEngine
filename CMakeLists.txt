cmake_minimum_required(VERSION 4.0)
project(NovaEngine)
include(CMakeDependentOption)

option(NOVA_ENGINE_CUSTOM_ENTRY_POINT "Provide a way to the user to define a custom entry point" OFF)
option(NOVA_ENGINE_INCLUDE_AUDIO "Compile engine with audio support" ON)
option(NOVA_ENGINE_INCLUDE_PHYSICS "Compile engine with physics support" ON)
option(NOVA_ENGINE_BUILD_ASSET_PACKER "Compile the Asset Packer program" ON)
cmake_dependent_option(NOVA_ENGINE_BUILD_D3D12 "Build the engine with D3D12 backend" ON WIN32 OFF)
option(NOVA_ENGINE_BUILD_VULKAN "Build the engine with Vulkan backend" ON)
option(NOVA_ENGINE_BUILD_OPENGL "Build the engine with OpenGL backend" ON)

if(NOVA_ENGINE_BUILD_VULKAN)
    include(CMake/Vulkan.cmake)
endif ()

add_subdirectory(Engine)
add_subdirectory(Editor)

if(NOVA_ENGINE_BUILD_ASSET_PACKER)
add_subdirectory(Programs/AssetPacker)
endif ()
