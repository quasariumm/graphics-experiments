cmake_minimum_required(VERSION 3.28)
project(dx12_wrapper)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

include(FetchContent)

FetchContent_Declare(
		DirectX-Headers
		GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
		GIT_TAG        v1.619.1
		GIT_SHALLOW    TRUE
)

FetchContent_Declare(
		GLFW
		GIT_REPOSITORY https://github.com/glfw/glfw.git
		GIT_TAG        3.4
		GIT_SHALLOW    TRUE
)

FetchContent_Declare(
		fmt
		GIT_REPOSITORY https://github.com/fmtlib/fmt.git
		GIT_TAG        12.1.0
		GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(DirectX-Headers GLFW fmt)

add_library(dx12_wrapper)

include(../common/cmake/setup_sources.cmake)

target_include_directories(dx12_wrapper PRIVATE
		../external
		${DirectX-Headers_SOURCE_DIR}/include
		${GLFW_SOURCE_DIR}/include
		${fmt_SOURCE_DIR}/include
)

target_link_libraries(dx12_wrapper PRIVATE
		Microsoft::DirectX-Headers
		Microsoft::DirectX-Guids
		glfw
)

set(DX12_LIBARY_PATH ${CMAKE_BINARY_DIR}/dx12_wrapper)