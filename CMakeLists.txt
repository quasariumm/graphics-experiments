cmake_minimum_required(VERSION 3.28)

include(common/cmake/setup_cxx.cmake)

project(graphics_experiments)

set(TESTPLATES_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(USE_IMPORT_STD FALSE CACHE INTERNAL "")
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.30"
		AND CMAKE_GENERATOR MATCHES "Ninja"
		AND ((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"  AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.36")
		OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "17.0")))
	set(USE_IMPORT_STD TRUE)
endif()

if (NOT USE_IMPORT_STD)
	add_subdirectory(std_module)
endif()

add_subdirectory(dx_wrapper)
add_subdirectory(dx_bare)
add_subdirectory(dx_imgui)
add_subdirectory(dx_meshlets)
add_subdirectory(dx_hardware_raytracing)