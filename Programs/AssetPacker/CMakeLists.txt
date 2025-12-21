include(../../CMake/Nova.cmake)

set(NOVA_ASSET_PACKER_SRC
        Source/AssetPackerApplication.cpp
        Source/AssetPackerApplication.h
        Source/AssetPack.cpp
        Source/AssetPack.h
)

add_executable(AssetPacker ${NOVA_ASSET_PACKER_SRC})
set_target_properties(AssetPacker PROPERTIES CXX_STANDARD 23)
target_sources(AssetPacker PRIVATE ${NOVA_ASSET_PACKER_SRC})
target_link_libraries(AssetPacker PUBLIC NovaEngine)
target_compile_definitions(AssetPacker PRIVATE NOVA_APPLICATION_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(AssetPacker PRIVATE Source)
if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    set_target_properties(AssetPacker PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()