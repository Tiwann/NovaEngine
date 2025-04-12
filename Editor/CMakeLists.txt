file(GLOB_RECURSE SOURCES *.cpp *.h)

set(CMAKE_CXX_STANDARD 20)
add_executable(NovaEditor ${SOURCES})
target_link_libraries(NovaEditor PRIVATE NovaEngine)
target_compile_definitions(NovaEditor PRIVATE NOVA_EDITOR)
target_include_directories(NovaEngine PRIVATE Source)