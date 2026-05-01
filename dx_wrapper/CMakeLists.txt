cmake_minimum_required(VERSION 3.28)
project(dx_wrapper)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

set(BUILD_SHARED_LIBS OFF)

#
# Fetch Repos
#

include(FetchContent)

add_compile_options(-march=native)

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
		GIT_TAG v1.619.1
		GIT_SHALLOW TRUE
)

FetchContent_Declare(
		DirectXMath
		GIT_REPOSITORY https://github.com/microsoft/DirectXMath.git
		GIT_TAG apr2025
		GIT_SHALLOW TRUE
)

FetchContent_Declare(
		IMath
		GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/Imath.git
		GIT_TAG v3.1.12
		GIT_SHALLOW TRUE
		OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
		OpenEXR
		GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/openexr.git
		GIT_TAG v3.4.10
		GIT_SHALLOW TRUE
)

FetchContent_Declare(
		DirectXTex
		GIT_REPOSITORY https://github.com/microsoft/DirectXTex.git
		GIT_TAG mar2026
		GIT_SHALLOW TRUE
)

FetchContent_Declare(
		fastgltf
		GIT_REPOSITORY https://github.com/spnda/fastgltf.git
		GIT_TAG v0.9.0
		GIT_SHALLOW TRUE
)

FetchContent_Declare(
		glm
		GIT_REPOSITORY https://github.com/g-truc/glm.git
		GIT_TAG 1.0.3
		GIT_SHALLOW TRUE
)

FetchContent_Declare(
		mikktspace
		GIT_REPOSITORY https://github.com/mmikk/MikkTSpace.git
		GIT_TAG 3e895b4
		GIT_SHALLOW TRUE
)

#
# OpenEXR
#
set(OPENEXR_BUILD_LIBS ON)
set(OPENEXR_BUILD_TOOLS OFF)
set(OPENEXR_BUILD_EXAMPLES OFF)

#
# DirectXTex
#
set(BUILD_TOOLS OFF)
set(BUILD_SAMPLE OFF)
set(BUILD_DX11 OFF)
set(ENABLE_OPENEXR_SUPPORT ON)

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
FetchContent_MakeAvailable(DirectX-Headers DirectXMath IMath OpenEXR DirectXTex fastgltf mikktspace glm)

add_library(mikktspace STATIC
		${mikktspace_SOURCE_DIR}/mikktspace.c
		${mikktspace_SOURCE_DIR}/mikktspace.h
)
target_include_directories(mikktspace PUBLIC ${mikktspace_SOURCE_DIR})

#
# Target settings
#

add_library(dx_wrapper STATIC)

target_compile_definitions(dx_wrapper PUBLIC DX_WRAPPER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/")
target_compile_options(dx_wrapper PUBLIC -march=native)

include(../common/cmake/setup_sources.cmake)

target_include_directories(dx_wrapper PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		../external
		${dxc_SOURCE_DIR}/
		${DirectX-Headers_SOURCE_DIR}/include/directx
)

target_link_libraries(dx_wrapper PUBLIC
		d3d12
		dxgi
		dxguid
		d3dcompiler
		Microsoft::DirectX-Headers
		Microsoft::DirectX-Guids
		DirectXMath
		DirectXTex
		fastgltf
		mikktspace
		glm::glm
)

target_compile_definitions(dx_wrapper PUBLIC BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}")

target_compile_definitions(dx_wrapper PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN)

target_compile_definitions(dx_wrapper PUBLIC GLM_ENABLE_EXPERIMENTAL GLM_FORCE_SWIZZLE)
target_compile_options(DirectXMath INTERFACE -Wno-defaulted-function-deleted)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-msvc)
	target_compile_definitions(dx_wrapper PUBLIC _AMD64_)
endif ()

add_custom_command(TARGET dx_wrapper POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${dxc_SOURCE_DIR}/bin/x64/dxcompiler.dll
		${dxc_SOURCE_DIR}/bin/x64/dxil.dll
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)