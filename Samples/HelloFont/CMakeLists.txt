file(GLOB_RECURSE SOURCES *.cpp *.h)

add_executable(HelloFont ${SOURCES})
set_target_properties(HelloFont PROPERTIES CXX_STANDARD 20)
target_link_libraries(HelloFont PRIVATE NovaEngine)
target_compile_definitions(HelloFont PRIVATE NOVA_APPLICATION_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
target_include_directories(HelloFont PRIVATE Source)
copy_imported_shared_lib(fmod-core HelloFont)
copy_imported_shared_lib(fmod-studio HelloFont)


if(CMAKE_BUILD_TYPE STREQUAL "Release" AND MSVC)
    set_target_properties(HelloFont PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()