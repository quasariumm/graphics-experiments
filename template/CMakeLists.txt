cmake_minimum_required(VERSION 3.28)

# Sets up the CXX target to C++23 with `import std` support and set up SIMD
include(../common/cmake/setup_cxx.cmake)

project(your_experiment)

# Turns off SHARED_LIBS compilation on following FetchContent calls and exposes a function for common compiler arguments
include(../common/cmake/setup_experiment.cmake)

add_executable(your_experiment)
setup_common_compiler_args(your_experiment)

# Globs all source files and puts them in the correct file set
include(../common/cmake/setup_sources.cmake)

# If it's a DX12 experiment, uncomment the following lines
# target_link_libraries(your_experiment PRIVATE dx_wrapper)
# copy_dxc_dlls(your_experiment)