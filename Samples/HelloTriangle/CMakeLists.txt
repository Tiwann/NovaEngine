file(GLOB_RECURSE SOURCES *.cpp *.h)

add_executable(HelloTriangle ${SOURCES})
set_target_properties(HelloTriangle PROPERTIES CXX_STANDARD 20)
target_link_libraries(HelloTriangle PRIVATE NovaEngine)
target_compile_definitions(HelloTriangle PRIVATE NOVA_APPLICATION_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
target_include_directories(HelloTriangle PRIVATE Source)
copy_imported_shared_lib(fmod-core HelloTriangle)
copy_imported_shared_lib(fmod-studio HelloTriangle)

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND MSVC)
    set_target_properties(HelloTriangle PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()