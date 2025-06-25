add_library(miniaudio STATIC)
file(GLOB_RECURSE MINIAUDIO_SRC *.h *.c)
target_sources(miniaudio PRIVATE ${MINIAUDIO_SRC})
target_include_directories(miniaudio PUBLIC include)