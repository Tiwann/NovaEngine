set(MODULE_DIR ${CMAKE_CURRENT_LIST_DIR})
file(GLOB_RECURSE SOURCES ${MODULE_DIR}/Source/*.cpp ${MODULE_DIR}/Source/*.h)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(NovaEditor ${SOURCES})

target_link_libraries(NovaEditor INTERFACE NovaEngine-Core)
target_link_libraries(NovaEditor INTERFACE NovaEngine-Physics)
target_link_libraries(NovaEditor INTERFACE NovaEngine-Audio)








