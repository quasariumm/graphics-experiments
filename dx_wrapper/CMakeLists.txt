cmake_minimum_required(VERSION 3.28)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_cxx.cmake)

project(dx_wrapper)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_testplate.cmake)
set(CMAKE_POLICY_VERSION_MINIMUM 3.10)

#
# Fetch Repos
#

include(FetchContent)

#
# DirectX Compiler
#

FetchContent_Declare(
		dxc
		URL https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.9.2602/dxc_2026_02_20.zip
		URL_HASH SHA256=a1e89031421cf3c1fca6627766ab3020ca4f962ac7e2caa7fab2b33a8436151e
)
FetchContent_MakeAvailable(dxc)

set(DIRECTX_DXC_TOOL "${dxc_SOURCE_DIR}/bin/x64/dxc.exe" CACHE PATH "" FORCE)

#
# DirectX Headers
#

FetchContent_Declare(
		DirectX-Headers
		GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
		GIT_TAG v1.619.1
		GIT_SHALLOW TRUE
		OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(DirectX-Headers)

#
# DirectX Math (for DirectXTex)
#

FetchContent_Declare(
		DirectXMath
		GIT_REPOSITORY https://github.com/microsoft/DirectXMath.git
		GIT_TAG apr2025
		GIT_SHALLOW TRUE
		OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(DirectXMath)
add_library(Microsoft::DirectXMath ALIAS DirectXMath)

#
# IMath (for OpenEXR)
#

FetchContent_Declare(
		IMath
		GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/Imath.git
		GIT_TAG v3.1.12
		GIT_SHALLOW TRUE
		OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(IMath)

#
# OpenEXR
#
FetchContent_Declare(
		OpenEXR
		GIT_REPOSITORY https://github.com/AcademySoftwareFoundation/openexr.git
		GIT_TAG v3.4.10
		GIT_SHALLOW TRUE
)

set(OPENEXR_BUILD_LIBS ON)
set(OPENEXR_BUILD_TOOLS OFF)
set(OPENEXR_BUILD_EXAMPLES OFF)

FetchContent_MakeAvailable(OpenEXR)

#
# DirectXTex
#

FetchContent_Declare(
		DirectXTex
		GIT_REPOSITORY https://github.com/microsoft/DirectXTex.git
		GIT_TAG mar2026
		GIT_SHALLOW TRUE
		OVERRIDE_FIND_PACKAGE
)

set(BUILD_TOOLS OFF)
set(BUILD_SAMPLE OFF)
set(BUILD_DX11 OFF)
set(ENABLE_OPENEXR_SUPPORT ON)
set(BUILD_TESTING OFF)

set(directxmath_DIR ${directxmath_BINARY_DIR} CACHE PATH "" FORCE)

# Fuck you Microsoft
set(CMAKE_SKIP_INSTALL_RULES ON)
FetchContent_MakeAvailable(DirectXTex)
set(CMAKE_SKIP_INSTALL_RULES OFF)

#
# FastGLTF
#

FetchContent_Declare(
		fastgltf
		GIT_REPOSITORY https://github.com/spnda/fastgltf.git
		GIT_TAG v0.9.0
		GIT_SHALLOW TRUE
)

set(FASTGLTF_COMPILE_AS_CPP20 ON)

FetchContent_MakeAvailable(fastgltf)

#
# GLM
#

FetchContent_Declare(
		glm
		GIT_REPOSITORY https://github.com/g-truc/glm.git
		GIT_TAG 1.0.3
		GIT_SHALLOW TRUE
)

set(GLM_ENABLE_CXX_20 ON)
set(GLM_ENABLE_SIMD_AVX2 ON)

FetchContent_MakeAvailable(glm)

#
# MikkTSpace
#

FetchContent_Declare(
		mikktspace
		GIT_REPOSITORY https://github.com/mmikk/MikkTSpace.git
		GIT_TAG 3e895b4
		GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(mikktspace)

add_library(mikktspace STATIC
		${mikktspace_SOURCE_DIR}/mikktspace.c
		${mikktspace_SOURCE_DIR}/mikktspace.h
)
target_include_directories(mikktspace PUBLIC ${mikktspace_SOURCE_DIR})

#
# Target settings
#

add_library(dx_wrapper STATIC)

setup_common_compiler_args(dx_wrapper)

target_compile_definitions(dx_wrapper PUBLIC DX_WRAPPER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/")

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_sources.cmake)

target_include_directories(dx_wrapper PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		../external
)
# This makes these have higher precedence over the system's headers
target_include_directories(dx_wrapper BEFORE PUBLIC
		${dxc_SOURCE_DIR}/inc
		${DirectX-Headers_SOURCE_DIR}/include/directx
)

target_link_libraries(dx_wrapper PUBLIC
		d3d12
		dxgi
		dxguid
		Microsoft::DirectX-Headers
		Microsoft::DirectX-Guids
		DirectXMath
		DirectXTex
		fastgltf
		mikktspace
		glm::glm
)

target_compile_definitions(dx_wrapper PUBLIC BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}")

target_compile_definitions(dx_wrapper PUBLIC GLM_ENABLE_EXPERIMENTAL)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(DirectXMath INTERFACE -Wno-defaulted-function-deleted)
endif()

if (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-gnu)
	target_compile_definitions(dx_wrapper PUBLIC _AMD64_)
endif ()

set(DXC_SOURCE_DIR ${dxc_SOURCE_DIR} CACHE PATH "" FORCE)
function(copy_dxc_dlls target)
add_custom_command(TARGET ${target} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${DXC_SOURCE_DIR}/bin/x64/dxcompiler.dll
		${DXC_SOURCE_DIR}/bin/x64/dxil.dll
		$<TARGET_FILE_DIR:${target}>)
endfunction()