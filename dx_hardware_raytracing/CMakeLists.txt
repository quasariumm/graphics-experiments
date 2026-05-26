cmake_minimum_required(VERSION 3.28)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_cxx.cmake)

project(dx_hardware_raytracing)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_experiment.cmake)

add_executable(dx_hardware_raytracing)

setup_common_compiler_args(dx_hardware_raytracing)

target_link_libraries(dx_hardware_raytracing PRIVATE dx_wrapper)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_sources.cmake)
copy_dxc_dlls(dx_hardware_raytracing)