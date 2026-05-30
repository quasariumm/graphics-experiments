cmake_minimum_required(VERSION 3.28)

include(common/cmake/setup_cxx.cmake)

project(graphics_experiments)

set(TESTPLATES_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(std_module)
add_subdirectory(dx_wrapper)
add_subdirectory(dx_bare)
add_subdirectory(dx_imgui)
add_subdirectory(dx_meshlets)
add_subdirectory(dx_hardware_raytracing)