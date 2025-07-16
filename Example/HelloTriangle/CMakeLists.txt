file(GLOB_RECURSE SOURCE Source/*.cpp Source/*.h)

add_executable(HelloTriangle)
set_target_properties(HelloTriangle PROPERTIES CXX_STANDARD 23)
target_sources(HelloTriangle PRIVATE ${SOURCE})
target_link_libraries(HelloTriangle PUBLIC NovaEngine)


if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    set_target_properties(HelloTriangle PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()

if(BUILD_SHARED_LIBS)
    copy_shared_lib(NovaEngine HelloTriangle)
endif ()