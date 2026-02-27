add_subdirectory(Vendor/ogg)
add_subdirectory(Vendor/vorbis)
add_subdirectory(Vendor/miniaudio)

set(KISSFFT_PKGCONFIG OFF)
set(KISSFFT_STATIC ON)
set(KISSFFT_TEST OFF)
set(KISSFFT_TOOLS OFF)
add_subdirectory(Vendor/kissfft)

add_subdirectory(Vendor/box2d)

set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_INSTALL_PDB OFF)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_FBX_IMPORTER   ON)
set(ASSIMP_BUILD_OBJ_IMPORTER   ON)
set(ASSIMP_BUILD_GLTF_IMPORTER  ON)
add_subdirectory(Vendor/assimp)


set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(Vendor/glfw)

set(IMGUI_DIR Vendor/imgui)
set(IMGUI_SRC ${IMGUI_DIR}/imconfig.h
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui.h
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_internal.h
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imstb_rectpack.h
        ${IMGUI_DIR}/imstb_textedit.h
        ${IMGUI_DIR}/imstb_truetype.h
        ${IMGUI_DIR}/backends/imgui_impl_glfw.h
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp)

if(NOVA_ENGINE_BUILD_VULKAN)
    list(APPEND IMGUI_SRC
            ${IMGUI_DIR}/backends/imgui_impl_vulkan.h
            ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp)
endif ()

if(NOVA_ENGINE_BUILD_D3D12)
    list(APPEND IMGUI_SRC
            ${IMGUI_DIR}/backends/imgui_impl_dx12.h
            ${IMGUI_DIR}/backends/imgui_impl_dx12.cpp)
endif ()

if(NOVA_ENGINE_BUILD_OPENGL)
    list(APPEND IMGUI_SRC
            ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
            ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)
endif ()

add_library(imgui STATIC)
set_target_properties(imgui PROPERTIES CXX_STANDARD 17)
target_sources(imgui PRIVATE ${IMGUI_SRC})
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui PRIVATE glfw Vulkan::Vulkan)

set(SPIRV_REFLECT_EXECUTABLE OFF)
set(SPIRV_REFLECT_INSTALL OFF)
set(SPIRV_REFLECT_STATIC_LIB ON)
add_subdirectory(Vendor/SPIRV-Reflect)

if(NOVA_ENGINE_BUILD_D3D12)
    include(FetchContent)
    FetchContent_Declare(
        directx-headers
        GIT_REPOSITORY git@github.com:microsoft/DirectX-Headers.git
        GIT_TAG v1.618.2)
    FetchContent_MakeAvailable(directx-headers)
    add_subdirectory(Vendor/D3D12MemoryAllocator)
endif ()
