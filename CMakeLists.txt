cmake_minimum_required(VERSION 3.28)

include(common/cmake/setup_cxx.cmake)

project(graphics_experiments)

add_subdirectory(dx_wrapper)
add_subdirectory(dx_bare)
add_subdirectory(dx_imgui)
add_subdirectory(dx_meshlets)