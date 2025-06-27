file(GLOB_RECURSE SOURCES *.cpp *.h)

add_executable(HelloTexture ${SOURCES})
set_target_properties(HelloTexture PROPERTIES CXX_STANDARD 20)
target_link_libraries(HelloTexture PRIVATE NovaEngine)
target_compile_definitions(HelloTexture PRIVATE NOVA_APPLICATION_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
target_include_directories(HelloTexture PRIVATE Source)
copy_imported_shared_lib(fmod-core HelloTexture)
copy_imported_shared_lib(fmod-studio HelloTexture)

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND MSVC)
    set_target_properties(HelloTexture PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()