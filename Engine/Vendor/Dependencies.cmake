###########################################################
#[[ GLFW ]]
###########################################################
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(BUILD_SHARED_LIBS OFF)
option(GLFW_INSTALL OFF)
add_subdirectory(Vendor/glfw)
set_target_properties(glfw PROPERTIES FOLDER Vendor)
###########################################################

###########################################################
#[[ ASSIMP ]]
###########################################################
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_INSTALL OFF)
option(ASSIMP_INSTALL_PDB OFF)
option(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
option(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)

set(ASSIMP_BUILD_FBX_EXPORTER   ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_EXPORTER   ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_EXPORTER  ON CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_FBX_IMPORTER   ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER   ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER  ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_BLEND_IMPORTER  ON CACHE BOOL "" FORCE)

add_subdirectory(Vendor/assimp)
set_target_properties(assimp PROPERTIES FOLDER Vendor)
###########################################################


############################################################
##[[ BOX2D ]]
############################################################
option(BOX2D_BUILD_DOCS OFF)
option(BOX2D_BUILD_TESTBED OFF)
option(BOX2D_BUILD_UNIT_TESTS OFF)
option(BOX2D_USER_SETTINGS OFF)
option(BUILD_SHARED_LIBS OFF)
add_subdirectory(Vendor/box2d)
set_target_properties(box2d PROPERTIES FOLDER Vendor)
###########################################################

###########################################################
#[[ JOLT ]]
###########################################################
option(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF)
add_subdirectory(Vendor/Jolt/Build)
set_target_properties(Jolt PROPERTIES FOLDER Vendor)

###########################################################
#[[ VULKAN MEMORY ALLOCATOR ]]
###########################################################
add_subdirectory(Vendor/VulkanMemoryAllocator)
set_target_properties(VulkanMemoryAllocator PROPERTIES FOLDER Vendor)
###########################################################

###########################################################
#[[ STB ]]
###########################################################
add_subdirectory(Vendor/stb)
set_target_properties(STB PROPERTIES FOLDER Vendor)
###########################################################

###########################################################
#[[ IMGUI ]]
###########################################################
set(CMAKE_CXX_STANDARD 17)
set(IMGUI_DIR Vendor/imgui)
add_library(ImGui STATIC
        ${IMGUI_DIR}/imconfig.h
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
        ${IMGUI_DIR}/backends/imgui_impl_dx12.h
        ${IMGUI_DIR}/backends/imgui_impl_dx12.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.h
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.h
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
)
target_include_directories(ImGui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(ImGui PRIVATE glfw Vulkan::Vulkan)
set_target_properties(ImGui PROPERTIES FOLDER Vendor)
###########################################################

###########################################################
#[[ IMGUIZMO ]]
###########################################################
set(IMGUIZMO_DIR Vendor/imguizmo)
set(IMGUIZMO_SOURCES
        ${IMGUIZMO_DIR}/GraphEditor.cpp
        ${IMGUIZMO_DIR}/GraphEditor.h
        ${IMGUIZMO_DIR}/ImCurveEdit.cpp
        ${IMGUIZMO_DIR}/ImCurveEdit.h
        ${IMGUIZMO_DIR}/ImGradient.cpp
        ${IMGUIZMO_DIR}/ImGradient.h
        ${IMGUIZMO_DIR}/ImGuizmo.h
        ${IMGUIZMO_DIR}/ImGuizmo.cpp
        ${IMGUIZMO_DIR}/ImSequencer.h
        ${IMGUIZMO_DIR}/ImSequencer.cpp
        ${IMGUIZMO_DIR}/ImZoomSlider.h
)
add_library(imguizmo STATIC ${IMGUIZMO_SOURCES})
target_link_libraries(imguizmo PRIVATE ImGui)
target_include_directories(imguizmo INTERFACE ${IMGUIZMO_DIR})
set_target_properties(imguizmo PROPERTIES FOLDER Vendor)
###########################################################


###########################################################
##[[ VULKAN ]]
###########################################################
find_package(Vulkan REQUIRED)


###########################################################
##[[ FMOD ]]
###########################################################
add_subdirectory(Vendor/FMODCore)
add_subdirectory(Vendor/FMODStudio)

###########################################################
##[[ UTFCPP ]]
###########################################################
add_subdirectory(Vendor/utfcpp)

###########################################################
##[[ GLAD ]]
###########################################################
add_subdirectory(Vendor/glad)
set_target_properties(glad PROPERTIES FOLDER Vendor)

###########################################################
##[[ MINIAUDIO ]]
###########################################################
add_subdirectory(Vendor/miniaudio)
set_target_properties(miniaudio PROPERTIES FOLDER Vendor)

###########################################################
##[[ VORBIS ]]
###########################################################
add_subdirectory(Vendor/ogg)
add_subdirectory(Vendor/vorbis)

###########################################################
##[[ json ]]
###########################################################
add_subdirectory(Vendor/json)

###########################################################
##[[ tinygltf ]]
###########################################################
add_subdirectory(Vendor/tinygltf)


###########################################################
##[[ DirectX-Headers ]]
###########################################################
add_subdirectory(Vendor/DirectX-Headers)

###########################################################
##[[ SLANG ]]
###########################################################
option(SLANG_ENABLE_EXAMPLES OFF)
option(SLANG_ENABLE_GFX OFF)
option(SLANG_ENABLE_SLANGC OFF)
option(SLANG_ENABLE_SLANG_RHI OFF)
option(SLANG_ENABLE_TESTS OFF)
option(SLANG_EXCLUDE_DAWN ON)
option(SLANG_EXCLUDE_TINT ON)
add_subdirectory(Vendor/slang)
