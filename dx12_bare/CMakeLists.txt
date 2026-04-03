cmake_minimum_required(VERSION 3.28)
project(dx12_bare)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

add_subdirectory(../dx12_library DX12_LIB_PATH)

add_executable(dx12_bare)

target_link_libraries(dx12_bare PRIVATE dx12_library)

include(../common/cmake/setup_sources.cmake)
