file(GLOB_RECURSE SOURCE Source/*.cpp Source/*.h)

add_executable(HelloTexture)
set_target_properties(HelloTexture PROPERTIES CXX_STANDARD 23)
target_sources(HelloTexture PRIVATE ${SOURCE})
target_link_libraries(HelloTexture PUBLIC NovaEngine)
target_compile_definitions(HelloTexture PRIVATE APPLICATION_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    set_target_properties(HelloTexture PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()

if(BUILD_SHARED_LIBS)
    copy_shared_lib(NovaEngine HelloTexture)
endif ()