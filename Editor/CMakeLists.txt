file(GLOB_RECURSE NOVA_EDITOR_SOURCES Source/*.cpp Source/*.h)

add_executable(NovaEditor)
set_target_properties(NovaEditor PROPERTIES CXX_STANDARD 23)
target_sources(NovaEditor PRIVATE ${NOVA_EDITOR_SOURCES})
target_link_libraries(NovaEditor PUBLIC NovaEngine)
target_compile_definitions(NovaEditor PRIVATE NOVA_APPLICATION_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    set_target_properties(NovaEditor PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()