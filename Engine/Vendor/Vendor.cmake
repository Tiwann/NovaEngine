find_package(Vulkan REQUIRED COMPONENTS volk)

add_subdirectory(Vendor/miniaudio)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_INSTALL OFF)

add_subdirectory(Vendor/glfw)