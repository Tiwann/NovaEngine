file(GLOB_RECURSE SOURCES *.cpp *.h)

include(FetchContent)

option(BOX2D_BUILD_DOCS OFF)
option(BOX2D_BUILD_TESTBED OFF)
option(BOX2D_BUILD_UNIT_TESTS OFF)
option(BOX2D_USER_SETTINGS OFF)
option(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG v2.4.2
)

FetchContent_Declare(
        Jolt
        GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
        GIT_BRANCH master
        SOURCE_SUBDIR Build
)

FetchContent_MakeAvailable(box2d Jolt)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(NovaEngine-Physics SHARED ${SOURCES})

target_link_libraries(NovaEngine-Physics
        PRIVATE NovaEngine-Core
        PRIVATE box2d
        PRIVATE Jolt)




