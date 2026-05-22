cmake_minimum_required(VERSION 3.28)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_cxx.cmake)

project(dx_meshlets)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_experiment.cmake)

FetchContent_Declare(
		meshoptimizer
		GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
		GIT_TAG v1.1
		GIT_SHALLOW TRUE
)
set(MESHOPT_INSTALL OFF)
FetchContent_MakeAvailable(meshoptimizer)

add_executable(dx_meshlets)

setup_common_compiler_args(dx_meshlets)

target_link_libraries(dx_meshlets PRIVATE meshoptimizer dx_wrapper)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_sources.cmake)
copy_dxc_dlls(dx_meshlets)