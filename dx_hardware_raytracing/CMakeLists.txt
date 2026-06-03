cmake_minimum_required(VERSION 3.28)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_cxx.cmake)

project(dx_hardware_raytracing)

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

add_executable(dx_hardware_raytracing)

target_compile_definitions(dx_hardware_raytracing PUBLIC TESTPLATE_HAS_IMGUI)

setup_common_compiler_args(dx_hardware_raytracing)

target_link_libraries(dx_hardware_raytracing PRIVATE dx_wrapper)

include(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/setup_sources.cmake)
copy_dlls(dx_hardware_raytracing)

#
# ImGUI
#
target_compile_definitions(dx_hardware_raytracing PRIVATE IMGUI_DEFINE_MATH_OPERATORS)
target_include_directories(dx_hardware_raytracing PRIVATE ${imgui_SOURCE_DIR})

target_sources(dx_hardware_raytracing
		PRIVATE
		${imgui_SOURCE_DIR}/imgui.cpp
		${imgui_SOURCE_DIR}/imgui_demo.cpp
		${imgui_SOURCE_DIR}/imgui_draw.cpp
		${imgui_SOURCE_DIR}/imgui_tables.cpp
		${imgui_SOURCE_DIR}/imgui_widgets.cpp
		external/patched_imgui_impl_dx12.cpp
		${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)