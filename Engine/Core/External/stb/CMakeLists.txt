add_library(stb STATIC stb.c)
target_include_directories(stb INTERFACE include)
target_compile_definitions(stb
        INTERFACE STB_IMAGE_IMPLEMENTATION
        INTERFACE STB_IMAGE_RESIZE_IMPLEMENTATION
        INTERFACE STB_IMAGE_WRITE_IMPLEMENTATION)