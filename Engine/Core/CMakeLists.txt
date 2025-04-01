file(GLOB_RECURSE SOURCES *.cpp *.h REMOVE_RECURSE Platform/*.cpp Platform/*.h)



if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(GLOB Platform/Windows/*.cpp Platform/Windows/*.h)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    file(GLOB Platform/Linux/*.cpp Platform/Linux/*.h)
else (CMAKE_SYSTEM_NAME STREQUAL "MacOS")
    file(GLOB Platform/MacOS/*.cpp Platform/MacOS/*.h)
endif ()

include(FetchContent)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(BUILD_SHARED_LIBS OFF)
option(GLFW_INSTALL OFF)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_BRANCH master
)

FetchContent_Declare(
        VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_BRANCH master
)

#[[option(YAML_CPP_BUILD_TOOLS OFF)
option(YAML_CPP_BUILD_CONTRIB OFF)
FetchContent_Declare(
        yaml
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_BRANCH master
)
FetchContent_MakeAvailable(yaml)]]


option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_INSTALL OFF)
option(ASSIMP_INSTALL_PDB OFF)
option(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
option(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_BRANCH master
)

#[[FetchContent_Declare(
        msdfgen
        GIT_REPOSITORY https://github.com/Chlumsky/msdfgen.git
        GIT_BRANCH master
)]]

option(SLANG_ENABLE_SLANGC OFF)
option(SLANG_ENABLE_TESTS OFF)
option(SLANG_LIB_TYPE STATIC)
option(SLANG_ENABLE_EXAMPLES OFF)

FetchContent_Declare(
        slang
        GIT_REPOSITORY https://github.com/shader-slang/slang.git
        GIT_BRANCH master
)

FetchContent_MakeAvailable(glfw VulkanMemoryAllocator assimp slang)

##add_subdirectory(External/stb)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(NovaEngine-Core SHARED ${SOURCES})

target_link_libraries(NovaEngine-Core PUBLIC glfw VulkanMemoryAllocator assimp stb)
target_include_directories(NovaEngine-Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})





