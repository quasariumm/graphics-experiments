# Standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# Import STD (C++23)
if (CMAKE_VERISON VERSION_GREATER_EQUAL "4.3.1")
	set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
elseif (CMAKE_VERSION VERSION_GREATER_EQUAL "4.0.3")
	set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
elseif (CMAKE_VERSION VERSION_GREATER_EQUAL "4.0.1")
	set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
else()
	set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
endif()

set(CMAKE_CXX_MODULE_STD ON)

add_compile_options(-target x86_64-w64-windows-gnu -pthread)
add_link_options(-target x86_64-w64-windows-gnu -pthread)

if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-stdlib=libc++)
	add_link_options(-stdlib=libc++)
endif ()

add_compile_options(-march=native)

# Because std.pcm is compiled without
set(CMAKE_DISABLE_FIND_PACKAGE_OpenMP TRUE)