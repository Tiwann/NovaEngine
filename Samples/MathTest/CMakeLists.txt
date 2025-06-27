
add_executable(MathTests Source/Main.cpp)
set_target_properties(MathTests PROPERTIES CXX_STANDARD 20)
target_link_libraries(MathTests PRIVATE NovaEngine)
target_include_directories(MathTests PRIVATE Source)