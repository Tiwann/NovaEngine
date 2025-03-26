set(MODULE_DIR ${CMAKE_CURRENT_LIST_DIR})
file(GLOB_RECURSE SOURCES ${MODULE_DIR}/Source/*.cpp ${MODULE_DIR}/Source/*.h)

include(FetchContent)

set(BOX2D_BUILD_DOCS OFF)
set(BOX2D_BUILD_TESTBED OFF)
set(BOX2D_BUILD_UNIT_TESTS OFF)
set(BOX2D_USER_SETTINGS OFF)
set(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG v2.4.2
)
FetchContent_MakeAvailable(box2d)

FetchContent_Declare(
        Jolt
        GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
        GIT_TAG v5.3.0
)

FetchContent_MakeAvailable(Jolt)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(NovaEngine-Physics SHARED ${SOURCES})

target_include_directories(NovaEngine-Physics
        INTERFACE Include
        PRIVATE ${box2d_SOURCE_DIR}/include
        PRIVATE ${Jolt_SOURCE_DIR})

target_link_libraries(NovaEngine-Physics
        PRIVATE NovaEngine-Core
        PUBLIC box2d)

target_compile_definitions(NovaEngine-Physics
        INTERFACE NOVAENGINE_PHYSICS_SHARED
        INTERFACE NOVAENGINE_PHYSICS_BUILD)







