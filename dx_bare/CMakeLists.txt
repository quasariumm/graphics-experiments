cmake_minimum_required(VERSION 3.28)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_cxx.cmake)

project(dx_bare)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_testplate.cmake)

add_executable(dx_bare)

setup_common_compiler_args(dx_bare)

target_link_libraries(dx_bare PRIVATE dx_wrapper)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_sources.cmake)
copy_dxc_dlls(dx_bare)