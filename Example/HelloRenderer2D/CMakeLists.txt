file(GLOB_RECURSE SOURCE Source/*.cpp Source/*.h)

add_executable(HelloRenderer2D)
set_target_properties(HelloRenderer2D PROPERTIES CXX_STANDARD 23)
target_sources(HelloRenderer2D PRIVATE ${SOURCE})
target_link_libraries(HelloRenderer2D PUBLIC NovaEngine)
target_compile_definitions(HelloRenderer2D PRIVATE APPLICATION_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    set_target_properties(HelloRenderer2D PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()

if(BUILD_SHARED_LIBS)
    copy_shared_lib(NovaEngine HelloRenderer2D)
endif ()