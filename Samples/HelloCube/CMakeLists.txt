file(GLOB_RECURSE SOURCES *.cpp *.h)

add_executable(HelloCube ${SOURCES})
set_target_properties(HelloCube PROPERTIES CXX_STANDARD 20)
target_link_libraries(HelloCube PRIVATE NovaEngine)
target_compile_definitions(HelloCube PRIVATE NOVA_APPLICATION_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
target_include_directories(HelloCube PRIVATE Source)
copy_imported_shared_lib(fmod-core HelloCube)
copy_imported_shared_lib(fmod-studio HelloCube)

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND MSVC)
    set_target_properties(HelloCube PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()