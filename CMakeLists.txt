cmake_minimum_required(VERSION 4.0)
project(NovaEngine)

include(CMake/Vulkan.cmake)
##include(CMake/FindVulkan.cmake)
##list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
option(NOVA_ENGINE_BUILD_EXAMPLES "Build example projects that uses Nova Engine" ON)
option(NOVA_ENGINE_CUSTOM_ENTRY_POINT "Provide a way to the user to define a custom entry point" ON)


add_subdirectory(Engine)

if(NOVA_ENGINE_BUILD_EXAMPLES)
    add_subdirectory(Example/HelloTriangle)
    add_subdirectory(Example/HelloCompute)
    add_subdirectory(Example/HelloTexture)
endif ()