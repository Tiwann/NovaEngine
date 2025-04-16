file(GLOB_RECURSE SOURCES *.cpp *.h)

set(CMAKE_CXX_STANDARD 20)
add_executable(HelloTriangle ${SOURCES})
target_link_libraries(HelloTriangle PRIVATE NovaEngine)
target_compile_definitions(HelloTriangle PRIVATE NOVA_APPLICATION_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
target_include_directories(HelloTriangle PRIVATE Source)
copy_imported_shared_lib(fmod-core HelloTriangle)
copy_imported_shared_lib(fmod-studio HelloTriangle)