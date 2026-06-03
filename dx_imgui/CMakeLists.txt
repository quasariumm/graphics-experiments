cmake_minimum_required(VERSION 3.28)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_cxx.cmake)

project(dx_imgui)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_experiment.cmake)

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

setup_common_compiler_args(dx_imgui)
target_compile_definitions(dx_imgui PUBLIC TESTPLATE_HAS_IMGUI)

target_link_libraries(dx_imgui PRIVATE dx_wrapper d3dcompiler dwmapi)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_sources.cmake)
copy_dlls(dx_imgui)

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
		external/patched_imgui_impl_dx12.cpp
		${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)