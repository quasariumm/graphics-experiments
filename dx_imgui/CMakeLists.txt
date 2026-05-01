cmake_minimum_required(VERSION 3.28)
project(dx_imgui)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

add_compile_options(-march=native)

#
# External modules
#

include(FetchContent)

FetchContent_Declare(
		imgui
		GIT_REPOSITORY https://github.com/ocornut/imgui.git
		GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

#
# Sources
#

add_executable(dx_imgui)

target_link_libraries(dx_imgui PRIVATE dx_wrapper)
target_compile_options(dx_imgui PUBLIC -march=native)

include(../common/cmake/setup_sources.cmake)

#
# ImGUI
#
target_compile_definitions(dx_imgui PRIVATE IMGUI_DEFINE_MATH_OPERATORS)
target_include_directories(dx_imgui PRIVATE ${imgui_SOURCE_DIR})

target_sources(dx_imgui
		PRIVATE
		${imgui_SOURCE_DIR}/imgui.cpp
		${imgui_SOURCE_DIR}/imgui_demo.cpp
		${imgui_SOURCE_DIR}/imgui_draw.cpp
		${imgui_SOURCE_DIR}/imgui_tables.cpp
		${imgui_SOURCE_DIR}/imgui_widgets.cpp
		${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
		${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)