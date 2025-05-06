set(CMAKE_CXX_STANDARD 20)
add_executable(MathTests Source/Main.cpp)
target_link_libraries(MathTests PRIVATE NovaEngine)
target_include_directories(MathTests PRIVATE Source)