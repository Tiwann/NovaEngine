set(MODULE_DIR ${CMAKE_CURRENT_LIST_DIR})
file(GLOB_RECURSE SOURCES ${MODULE_DIR}/Source/*.cpp ${MODULE_DIR}/Source/*.h)

include(FetchContent)
FetchContent_Declare(
	box2d
	GIT_REPOSITORY https://github.com/erincatto/box2d.git
	GIT_TAG v3.0.0
)
FetchContent_MakeAvailable(box2d)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(NovaEngine-Audio SHARED ${SOURCES})

target_link_libraries(NovaEngine-Audio PRIVATE NovaEngine-Core)
