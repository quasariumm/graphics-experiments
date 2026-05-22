# DX Wrapper

A wrapper around DirectX12 make with C++ modules. The wrapper consists of multiple module groups:

## Core
This houses all classes that are at the core of the rest of the wrapper. This includes common functions along with the 
graphics device, logging, input and transforms.

## External
Custom-made modules which export types from an external library, because sadly far from all libraries have support for it.

## glTF
A glTF model loader wrapping `fastgltf` to achieve fast model loading.

## Helpers
Utility classes and functions for all sorts of things. They help with streamlining the most common of DX12 tasks.

## Rendering
Exposes classes and functions to create shader-related things.

## Resources
Exposes classes of GPU-based resources.