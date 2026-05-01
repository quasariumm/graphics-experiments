cmake_minimum_required(VERSION 3.28)
project(dx_bare)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

add_executable(dx_bare)

target_link_libraries(dx_bare PRIVATE dx_wrapper)

include(../common/cmake/setup_sources.cmake)
