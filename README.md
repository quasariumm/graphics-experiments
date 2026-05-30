# Graphics Experiments
This is a library of personal projects with which I experiment with graphics-related concepts.

## Experiments
### DX12
- `dx_wrapper`: A wrapper library used by all other DX12 experiments
- `dx_bare`: A demo with a simple camera and PBR Forward rendering pipeline
- `dx_imgui`: A demo that renders Dear ImGUI windows
- `dx_meshlets`: A demo in which I try out clustered geometry rendering for the first time

## Building
This the projects use C++23 with modules support. The list of compilers that support this are:
- [Windows] MSVC 14.34 or newer
- [MinGW, Linux] GCC 14 or newer
- [MinGW, Linux] Clang 16.0 or newer

For a more up-to-date list, please refer to the [CMake docs](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html#compiler-support) 

When building on Windows, I recomment using MSVC for all DX12 projects, since the DX12 debug layer messages print to the LLDB console and are not visible (as far as I can see) on GDB.

You can compile the project by running the following commands:
```shell
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_CXX_COMPILER="/path/to/cxx_compiler" -DCMAKE_C_COMPILER="/path/to/c_compiler"
cmake --build . --target <experiment_name>
```

## Consuming
The wrappers and libaries made in this project are meant to be consumed by other experiments *only*. I have not yet had the time
to properly set it up to integrate properly with FetchContent. If you wish to use a wrapper or libary, you can either:
- Copy the directory of it to your project's libraries directory
- Make your project in this repo. The `template` directory shows the workflow with CMake. The template is made to be easy to set up, since most setup-steps
  are abstracted away in utility files in `common/cmake`.
