file(GLOB_RECURSE SOURCE Source/*.cpp Source/*.h)

add_executable(HelloAudio)
set_target_properties(HelloAudio PROPERTIES CXX_STANDARD 23)
target_sources(HelloAudio PRIVATE ${SOURCE})
target_link_libraries(HelloAudio PUBLIC NovaEngine)
target_compile_definitions(HelloAudio PRIVATE APPLICATION_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    set_target_properties(HelloAudio PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()

if(BUILD_SHARED_LIBS)
    copy_shared_lib(NovaEngine HelloAudio)
endif ()