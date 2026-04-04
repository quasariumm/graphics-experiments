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
		DirectXMath
		GIT_REPOSITORY https://github.com/microsoft/DirectXMath.git
		GIT_TAG        main
		GIT_SHALLOW    TRUE
)

FetchContent_Declare(
		DirectXTK12
		GIT_REPOSITORY https://github.com/microsoft/DirectXTK12.git
		GIT_TAG        main
		GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(DirectX-Headers DirectXMath DirectXTK12)

add_library(dx12_wrapper)

include(../common/cmake/setup_sources.cmake)

target_include_directories(dx12_wrapper PUBLIC
		${CMAKE_SOURCE_DIR}/include
		../external
		${DirectX-Headers_SOURCE_DIR}/include
		${DirectXTK12_SOURCE_DIR}/Inc
		${fmt_SOURCE_DIR}/include
)

target_link_libraries(dx12_wrapper PUBLIC
		d3d12.lib
		dxgi.lib
		dxguid.lib
		Microsoft::DirectX-Headers
		Microsoft::DirectX-Guids
		DirectXMath
		DirectXTK12
)

set(DX12_LIBARY_PATH ${CMAKE_BINARY_DIR}/dx12_wrapper)