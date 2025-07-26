file(GLOB_RECURSE SOURCE Source/*.cpp Source/*.h)

add_executable(HelloCompute)
set_target_properties(HelloCompute PROPERTIES CXX_STANDARD 23)
target_sources(HelloCompute PRIVATE ${SOURCE})
target_link_libraries(HelloCompute PUBLIC NovaEngine)
target_compile_definitions(HelloCompute PRIVATE APPLICATION_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    set_target_properties(HelloCompute PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()

if(BUILD_SHARED_LIBS)
    copy_shared_lib(NovaEngine HelloCompute)
endif ()