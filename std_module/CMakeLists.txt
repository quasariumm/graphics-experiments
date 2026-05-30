cmake_minimum_required(VERSION 3.28)

project(std_module)

include(../common/cmake/setup_cxx.cmake)

add_library(std_module STATIC)
target_sources(std_module PUBLIC FILE_SET CXX_MODULES FILES std.cppm)

include(../common/cmake/setup_experiment.cmake)

setup_common_compiler_args(std_module)

target_compile_definitions(BUILD_CUSTOM_STD_MODULE)