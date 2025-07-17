cmake_minimum_required(VERSION 4.0)
project(NovaEngine)

include(CMake/FindVulkan.cmake)
option(NOVA_ENGINE_BUILD_EXAMPLES "Build example projects that uses Nova Engine" ON)
option(NOVA_ENGINE_CUSTOM_ENTRY_POINT "Provide a way to the user to define a custom entry point" ON)


function(copy_shared_lib IMPORTED_TARGET CONSUMER_TARGET)
    add_custom_command(TARGET ${CONSUMER_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${IMPORTED_TARGET}> $<TARGET_FILE_DIR:${CONSUMER_TARGET}>
            COMMENT "Copying shared library from $<TARGET_FILE:${IMPORTED_TARGET}> to $<TARGET_FILE_DIR:${CONSUMER_TARGET}>")

endfunction()

add_subdirectory(Engine)

if(NOVA_ENGINE_BUILD_EXAMPLES)
add_subdirectory(Example/HelloTriangle)
endif ()