add_library(STB STATIC src/stb.c)
target_include_directories(STB PUBLIC include)
target_compile_definitions(STB
        PRIVATE STB_IMAGE_IMPLEMENTATION
        PRIVATE STB_IMAGE_RESIZE_IMPLEMENTATION
        PRIVATE STB_IMAGE_WRITE_IMPLEMENTATION)