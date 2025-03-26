set(MODULE_DIR ${CMAKE_CURRENT_LIST_DIR})
file(GLOB_RECURSE SOURCES ${MODULE_DIR}/**.cpp ${MODULE_DIR}/**.h)

include(FetchContent)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(NovaEngine-Core SHARED ${SOURCES})


target_include_directories(NovaEngine-Core
        PRIVATE ${glfw_SOURCE_DIR}/include)


target_compile_definitions(NovaEngine-Core
        INTERFACE NOVAENGINE_CORE_SHARED
        INTERFACE NOVAENGINE_CORE_BUILD)

target_link_libraries(NovaEngine-Core PUBLIC glfw)






