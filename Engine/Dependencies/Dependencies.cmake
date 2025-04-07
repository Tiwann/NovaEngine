###########################################################
#[[ GLFW ]]
###########################################################
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(BUILD_SHARED_LIBS OFF)
option(GLFW_INSTALL OFF)
add_subdirectory(Dependencies/glfw)
set_target_properties(glfw PROPERTIES FOLDER Dependencies)
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

add_subdirectory(Dependencies/assimp)
set_target_properties(assimp PROPERTIES FOLDER Dependencies)
###########################################################


############################################################
##[[ BOX2D ]]
############################################################
option(BOX2D_BUILD_DOCS OFF)
option(BOX2D_BUILD_TESTBED OFF)
option(BOX2D_BUILD_UNIT_TESTS OFF)
option(BOX2D_USER_SETTINGS OFF)
option(BUILD_SHARED_LIBS OFF)
add_subdirectory(Dependencies/box2d)
set_target_properties(box2d PROPERTIES FOLDER Dependencies)
###########################################################

###########################################################
#[[ JOLT ]]
###########################################################
add_subdirectory(Dependencies/Jolt/Build)
set_target_properties(Jolt PROPERTIES FOLDER Dependencies)

###########################################################
#[[ VULKAN MEMORY ALLOCATOR ]]
###########################################################
add_subdirectory(Dependencies/VulkanMemoryAllocator)
set_target_properties(VulkanMemoryAllocator PROPERTIES FOLDER Dependencies)
###########################################################

###########################################################
#[[ STB ]]
###########################################################
add_subdirectory(Dependencies/stb)
set_target_properties(stb PROPERTIES FOLDER Dependencies)
###########################################################

###########################################################
#[[ NVRHI ]]
###########################################################
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    option(NVRHI_WITH_DX12 ON)
endif ()
option(NVRHI_WITH_VULKAN ON)
add_subdirectory(Dependencies/NVRHI)
set_target_properties(nvrhi PROPERTIES FOLDER Dependencies)
###########################################################

###########################################################
#[[ IMGUI ]]
###########################################################
set(CMAKE_CXX_STANDARD 17)
set(IMGUI_DIR Dependencies/imgui)
add_library(imgui STATIC
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
)
target_include_directories(imgui INTERFACE Dependencies/imgui)
set_target_properties(imgui PROPERTIES FOLDER Dependencies)
###########################################################

###########################################################
#[[ IMGUIZMO ]]
###########################################################
set(IMGUIZMO_DIR Dependencies/imguizmo)
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
target_link_libraries(imguizmo PRIVATE imgui)
target_include_directories(imguizmo INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(imguizmo PROPERTIES FOLDER Dependencies)
###########################################################


###########################################################
##[[ VULKAN ]]
###########################################################
find_package(Vulkan REQUIRED)