set(CMAKE_CXX_STANDARD 11)
add_library(tinygltf STATIC)
target_include_directories(tinygltf PUBLIC include)
target_link_libraries(tinygltf PRIVATE STB nlohmann_json)
target_sources(tinygltf PRIVATE include/tinygltf/tinygltf.h src/tinygltf.cpp)