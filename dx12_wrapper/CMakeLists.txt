cmake_minimum_required(VERSION 3.28)
project(dx12_wrapper)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

#
# Fetch Repos
#

include(FetchContent)

FetchContent_Declare(
		dxc
		URL https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.9.2602/dxc_2026_02_20.zip
		URL_HASH SHA256=a1e89031421cf3c1fca6627766ab3020ca4f962ac7e2caa7fab2b33a8436151e
)
FetchContent_MakeAvailable(dxc)

set(DIRECTX_DXC_TOOL "${dxc_SOURCE_DIR}/bin/x64/dxc.exe" CACHE PATH "" FORCE)

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

FetchContent_Declare(
		fastgltf
		GIT_REPOSITORY https://github.com/spnda/fastgltf.git
		GIT_TAG        main
		GIT_SHALLOW    TRUE
)

FetchContent_Declare(
		glm
		GIT_REPOSITORY https://github.com/g-truc/glm.git
		GIT_TAG        1.0.3
		GIT_SHALLOW    TRUE
)

FetchContent_Declare(
		mikktspace
		GIT_REPOSITORY https://github.com/mmikk/MikkTSpace.git
		GIT_TAG        master
		GIT_SHALLOW    TRUE
)

#
# fastgltf
#

set(FASTGLTF_COMPILE_AS_CPP20 ON)

#
# GLM
#

set(GLM_ENABLE_CXX_20 ON)
set(GLM_ENABLE_SIMD_AVX2 ON)

set(CMAKE_POLICY_VERSION_MINIMUM 3.10)
FetchContent_MakeAvailable(DirectX-Headers DirectXMath DirectXTK12 fastgltf mikktspace glm)

add_library(mikktspace STATIC
		${mikktspace_SOURCE_DIR}/mikktspace.c
		${mikktspace_SOURCE_DIR}/mikktspace.h
)
target_include_directories(mikktspace PUBLIC ${mikktspace_SOURCE_DIR})

#
# Target settings
#

add_library(dx12_wrapper)

include(../common/cmake/setup_sources.cmake)

target_include_directories(dx12_wrapper PUBLIC
		${CMAKE_SOURCE_DIR}/include
		../external
		${dxc_SOURCE_DIR}/inc
		${DirectX-Headers_SOURCE_DIR}/include
		${DirectX-Headers_SOURCE_DIR}/include/directx
		${DirectXTK12_SOURCE_DIR}/Inc
		${fmt_SOURCE_DIR}/include
		${fastgltf_SOURCE_DIR}/include
		${glm_SOURCE_DIR}
)

target_link_libraries(dx12_wrapper PUBLIC
		d3d12
		dxgi
		dxguid
		d3dcompiler
		Microsoft::DirectX-Headers
		Microsoft::DirectX-Guids
		DirectXMath
		DirectXTK12
		fastgltf
		mikktspace
		glm::glm
)

target_compile_definitions(dx12_wrapper PUBLIC BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}")

target_compile_definitions(dx12_wrapper PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN)

target_compile_definitions(dx12_wrapper PUBLIC GLM_FORCE_SWIZZLE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-msvc)
	target_compile_definitions(dx12_wrapper PUBLIC _AMD64_)
endif()

add_custom_command(TARGET dx12_wrapper POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${dxc_SOURCE_DIR}/bin/x64/dxcompiler.dll
		${dxc_SOURCE_DIR}/bin/x64/dxil.dll
		$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)