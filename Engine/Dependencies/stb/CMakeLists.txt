add_library(stb STATIC src/stb.c)
target_include_directories(stb PUBLIC include)
target_compile_definitions(stb
        PRIVATE STB_IMAGE_IMPLEMENTATION
        PRIVATE STB_IMAGE_RESIZE_IMPLEMENTATION
        PRIVATE STB_IMAGE_WRITE_IMPLEMENTATION)