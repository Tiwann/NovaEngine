﻿cmake_minimum_required(VERSION 3.10...4.0)
project(NovaEngine C CXX)
include(FindVulkan.cmake)
include(AutoCopy.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(Samples)