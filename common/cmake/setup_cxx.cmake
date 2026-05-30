# Standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

if (MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-target x86_64-w64-windows-gnu -pthread)
	add_link_options(-target x86_64-w64-windows-gnu -pthread)
	add_compile_options(-stdlib=libc++)
	add_link_options(-stdlib=libc++)
endif ()

if (CMAKE_CXX_COMPLER_ID STREQUAL "Clang")
	add_compile_options(-march=native)
else()
	add_compile_options(/arch:AVX2)
endif()

# Because std.pcm is compiled without
set(CMAKE_DISABLE_FIND_PACKAGE_OpenMP TRUE)