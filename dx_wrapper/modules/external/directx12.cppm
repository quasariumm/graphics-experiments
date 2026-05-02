// NOLINTBEGIN
module;

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

export module dx_wrapper.external.directx12;
export import dx_wrapper.external.win32_dx_common;

export using ::Microsoft::WRL::ComPtr;

#pragma region Common
// =========================================================================================================
// Common interfaces
// =========================================================================================================
export using ::ID3D12Device2;
export using ::ID3D12GraphicsCommandList;
export using ::ID3D12CommandList;
export using ::ID3D12CommandAllocator;
export using ::ID3D12CommandQueue;
export using ::ID3D12CommandSignature;
export using ::ID3D12Fence;
export using ::ID3D12Fence1;
export using ::ID3D12DescriptorHeap;
export using ::CD3DX12_RESOURCE_BARRIER;

static constexpr float D3D12_FLOAT32_MAX_val = D3D12_FLOAT32_MAX;
#undef D3D12_FLOAT32_MAX
export constexpr float D3D12_FLOAT32_MAX = D3D12_FLOAT32_MAX_val;

export using ::D3D12_DEFAULT;

// =========================================================================================================
// Common functions
// =========================================================================================================
export using ::CommandListCast;
export using ::UpdateSubresources;
export using ::GetRequiredIntermediateSize;

// =========================================================================================================
// Enums
// =========================================================================================================
// D3D12_COMMAND_LIST_TYPE
export using ::D3D12_COMMAND_LIST_TYPE;
export using ::D3D12_COMMAND_LIST_TYPE_BUNDLE;
export using ::D3D12_COMMAND_LIST_TYPE_COMPUTE;
export using ::D3D12_COMMAND_LIST_TYPE_COPY;
export using ::D3D12_COMMAND_LIST_TYPE_DIRECT;
export using ::D3D12_COMMAND_LIST_TYPE_NONE;

// D3D12_FENCE_FLAGS
export using ::D3D12_FENCE_FLAGS;
export using ::D3D12_FENCE_FLAG_NON_MONITORED;
export using ::D3D12_FENCE_FLAG_NONE;
export using ::D3D12_FENCE_FLAG_SHARED;
export using ::D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER;
#pragma endregion

#pragma region Pipelines
// =========================================================================================================
// Root Signature
// =========================================================================================================
export using ::ID3D12RootSignature;

export using ::D3D12_STATIC_SAMPLER_DESC;
export using ::D3D12_ROOT_PARAMETER;
export using ::D3D12_ROOT_PARAMETER1;
export using ::D3D12_DESCRIPTOR_RANGE1;

export using ::D3D12_FEATURE_DATA_ROOT_SIGNATURE;
export using ::D3D12_VERSIONED_ROOT_SIGNATURE_DESC;
export using ::CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC;
export using ::D3DX12SerializeVersionedRootSignature;

export using ::D3D12_FEATURE_ROOT_SIGNATURE;
export using ::D3D12_FEATURE_ROOT_SIGNATURE;

export inline void operator|=(D3D12_ROOT_SIGNATURE_FLAGS& a, const D3D12_ROOT_SIGNATURE_FLAGS& b)
{ a = (D3D12_ROOT_SIGNATURE_FLAGS)((UINT)a | (UINT)b); }

// =========================================================================================================
// Pipeline State
// =========================================================================================================
export using ::ID3D12PipelineState;

export using ::D3D12_INPUT_ELEMENT_DESC;
export using ::D3D12_SHADER_BYTECODE;
export using ::CD3DX12_SHADER_BYTECODE;

export using ::D3D12_RASTERIZER_DESC;
export using ::CD3DX12_RASTERIZER_DESC;

export using ::D3D12_BLEND_DESC;
export using ::CD3DX12_BLEND_DESC;

export using ::D3D12_DEPTH_STENCIL_DESC;
export using ::CD3DX12_DEPTH_STENCIL_DESC;

export using ID3DBlob = ::ID3D10Blob;

export using ::D3D12_GRAPHICS_PIPELINE_STATE_DESC;
export using ::D3D12_COMPUTE_PIPELINE_STATE_DESC;
export using ::D3D12_PIPELINE_STATE_STREAM_DESC;
export using ::D3DX12_MESH_SHADER_PIPELINE_STATE_DESC;

static constexpr int D3D12_APPEND_ALIGNED_ELEMENT_val = D3D12_APPEND_ALIGNED_ELEMENT;
#undef D3D12_APPEND_ALIGNED_ELEMENT
export constexpr int D3D12_APPEND_ALIGNED_ELEMENT = D3D12_APPEND_ALIGNED_ELEMENT_val;

// =========================================================================================================
// Enums
// =========================================================================================================
// D3D12_INPUT_CLASSIFICATION
export using ::D3D12_INPUT_CLASSIFICATION;
export using ::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
export using ::D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;

// D3D12_CULL_MODE
export using ::D3D12_CULL_MODE;
export using ::D3D12_CULL_MODE_BACK;
export using ::D3D12_CULL_MODE_FRONT;
export using ::D3D12_CULL_MODE_NONE;

// D3D12_PRIMITIVE_TOPOLOGY_TYPE
export using ::D3D12_PRIMITIVE_TOPOLOGY_TYPE;
export using ::D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
export using ::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
export using ::D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
export using ::D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
export using ::D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;

// D3D12_BLEND
export using ::D3D12_BLEND;
export using ::D3D12_BLEND_ALPHA_FACTOR;
export using ::D3D12_BLEND_BLEND_FACTOR;
export using ::D3D12_BLEND_INV_ALPHA_FACTOR;
export using ::D3D12_BLEND_INV_BLEND_FACTOR;
export using ::D3D12_BLEND_DEST_ALPHA;
export using ::D3D12_BLEND_DEST_COLOR;
export using ::D3D12_BLEND_INV_DEST_ALPHA;
export using ::D3D12_BLEND_INV_DEST_COLOR;
export using ::D3D12_BLEND_INV_SRC1_ALPHA;
export using ::D3D12_BLEND_INV_SRC1_COLOR;
export using ::D3D12_BLEND_INV_SRC_ALPHA;
export using ::D3D12_BLEND_INV_SRC_COLOR;
export using ::D3D12_BLEND_SRC1_ALPHA;
export using ::D3D12_BLEND_SRC1_COLOR;
export using ::D3D12_BLEND_SRC_ALPHA;
export using ::D3D12_BLEND_SRC_ALPHA_SAT;
export using ::D3D12_BLEND_SRC_COLOR;

// D3D12_BLEND_OP
export using ::D3D12_BLEND_OP;
export using ::D3D12_BLEND_OP_ADD;
export using ::D3D12_BLEND_OP_REV_SUBTRACT;
export using ::D3D12_BLEND_OP_MAX;
export using ::D3D12_BLEND_OP_MIN;
export using ::D3D12_BLEND_OP_SUBTRACT;

// D3D12_COLOR_WRITE_ENABLE
export using ::D3D12_COLOR_WRITE_ENABLE;
export using ::D3D12_COLOR_WRITE_ENABLE_RED;
export using ::D3D12_COLOR_WRITE_ENABLE_GREEN;
export using ::D3D12_COLOR_WRITE_ENABLE_BLUE;
export using ::D3D12_COLOR_WRITE_ENABLE_ALPHA;
export using ::D3D12_COLOR_WRITE_ENABLE_ALL;

// D3D12_ROOT_SIGNATURE_FLAGS
export using ::D3D12_ROOT_SIGNATURE_FLAGS;
export using ::D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
export using ::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
export using ::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
export using ::D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
export using ::D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
export using ::D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
export using ::D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
export using ::D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
export using ::D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
export using ::D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
export using ::D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
export using ::D3D12_ROOT_SIGNATURE_FLAG_NONE;
export using ::D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;

// D3D_ROOT_SIGNATURE_VERSION
export using ::D3D_ROOT_SIGNATURE_VERSION;
export using ::D3D_ROOT_SIGNATURE_VERSION_1;
export using ::D3D_ROOT_SIGNATURE_VERSION_1_0;
export using ::D3D_ROOT_SIGNATURE_VERSION_1_1;
export using ::D3D_ROOT_SIGNATURE_VERSION_1_2;

// D3D12_SHADER_VISIBILITY
export using ::D3D12_SHADER_VISIBILITY;
export using ::D3D12_SHADER_VISIBILITY_ALL;
export using ::D3D12_SHADER_VISIBILITY_VERTEX;
export using ::D3D12_SHADER_VISIBILITY_HULL;
export using ::D3D12_SHADER_VISIBILITY_DOMAIN;
export using ::D3D12_SHADER_VISIBILITY_GEOMETRY;
export using ::D3D12_SHADER_VISIBILITY_PIXEL;
export using ::D3D12_SHADER_VISIBILITY_AMPLIFICATION;
export using ::D3D12_SHADER_VISIBILITY_MESH;

// D3D12_DESCRIPTOR_RANGE_TYPE
export using ::D3D12_DESCRIPTOR_RANGE_TYPE;
export using ::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
export using ::D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
export using ::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
export using ::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;

// D3D12_FILTER
export using ::D3D12_FILTER;
export using ::D3D12_FILTER_MIN_MAG_MIP_POINT;
export using ::D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
export using ::D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
export using ::D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
export using ::D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
export using ::D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
export using ::D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
export using ::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
export using ::D3D12_FILTER_MIN_MAG_ANISOTROPIC_MIP_POINT;
export using ::D3D12_FILTER_ANISOTROPIC;
export using ::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
export using ::D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
export using ::D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
export using ::D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
export using ::D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
export using ::D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
export using ::D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
export using ::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
export using ::D3D12_FILTER_COMPARISON_MIN_MAG_ANISOTROPIC_MIP_POINT;
export using ::D3D12_FILTER_COMPARISON_ANISOTROPIC;
export using ::D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
export using ::D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
export using ::D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
export using ::D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
export using ::D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
export using ::D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
export using ::D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
export using ::D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
export using ::D3D12_FILTER_MINIMUM_MIN_MAG_ANISOTROPIC_MIP_POINT;
export using ::D3D12_FILTER_MINIMUM_ANISOTROPIC;
export using ::D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
export using ::D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
export using ::D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
export using ::D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
export using ::D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
export using ::D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
export using ::D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
export using ::D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
export using ::D3D12_FILTER_MAXIMUM_MIN_MAG_ANISOTROPIC_MIP_POINT;
export using ::D3D12_FILTER_MAXIMUM_ANISOTROPIC;

// D3D12_TEXTURE_ADDRESS_MODE
export using ::D3D12_TEXTURE_ADDRESS_MODE;
export using ::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
export using ::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
export using ::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
export using ::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
export using ::D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;

// D3D12_STATIC_BORDER_COLOR
export using ::D3D12_STATIC_BORDER_COLOR;
export using ::D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
export using ::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
export using ::D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
export using ::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK_UINT;
export using ::D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE_UINT;

// D3D12_COMPARISON_FUNC
export using ::D3D12_COMPARISON_FUNC;
export using ::D3D12_COMPARISON_FUNC_NEVER;
export using ::D3D12_COMPARISON_FUNC_LESS;
export using ::D3D12_COMPARISON_FUNC_EQUAL;
export using ::D3D12_COMPARISON_FUNC_LESS_EQUAL;
export using ::D3D12_COMPARISON_FUNC_GREATER;
export using ::D3D12_COMPARISON_FUNC_NOT_EQUAL;
export using ::D3D12_COMPARISON_FUNC_GREATER_EQUAL;
export using ::D3D12_COMPARISON_FUNC_ALWAYS;

// D3D12_ROOT_PARAMETER_TYPE
export using ::D3D12_ROOT_PARAMETER_TYPE;
export using ::D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
export using ::D3D12_ROOT_PARAMETER_TYPE_SRV;
export using ::D3D12_ROOT_PARAMETER_TYPE_UAV;
export using ::D3D12_ROOT_PARAMETER_TYPE_CBV;
export using ::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
#pragma endregion

#pragma region Descriptors
// =========================================================================================================
// Heap handles
// =========================================================================================================
export using ::D3D12_CPU_DESCRIPTOR_HANDLE;
export using ::CD3DX12_CPU_DESCRIPTOR_HANDLE;
export using ::D3D12_GPU_DESCRIPTOR_HANDLE;
export using ::CD3DX12_GPU_DESCRIPTOR_HANDLE;

// =========================================================================================================
// Descriptor Heaps
// =========================================================================================================
export using ::D3D12_DESCRIPTOR_HEAP_DESC;

export using ::D3D12_DESCRIPTOR_HEAP_TYPE;

// =========================================================================================================
// Enums
// =========================================================================================================
// D3D12_DESCRIPTOR_HEAP_FLAGS
export using ::D3D12_DESCRIPTOR_HEAP_FLAGS;
export using ::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
export using ::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

// D3D12_HEAP_TYPE
export using ::D3D12_DESCRIPTOR_HEAP_TYPE;
export using ::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
export using ::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
export using ::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
export using ::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;

// D3D12_HEAP_FLAGS
export using ::D3D12_HEAP_FLAGS;
export using ::D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;
export using ::D3D12_HEAP_FLAG_ALLOW_DISPLAY;
export using ::D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
export using ::D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
export using ::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
export using ::D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS;
export using ::D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH;
export using ::D3D12_HEAP_FLAG_CREATE_NOT_RESIDENT;
export using ::D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;
export using ::D3D12_HEAP_FLAG_DENY_BUFFERS;
export using ::D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
export using ::D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
export using ::D3D12_HEAP_FLAG_HARDWARE_PROTECTED;
export using ::D3D12_HEAP_FLAG_NONE;
export using ::D3D12_HEAP_FLAG_SHARED;
export using ::D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER;
export using ::D3D12_HEAP_FLAG_TOOLS_USE_MANUAL_WRITE_TRACKING;
#pragma endregion

#pragma region Resources
// =========================================================================================================
// Resources
// =========================================================================================================
export using ::ID3D12Resource;
export using ::D3D12_HEAP_PROPERTIES;
export using ::D3D12_HEAP_PROPERTIES;
export using ::CD3DX12_HEAP_PROPERTIES;
export using ::D3D12_RESOURCE_DESC;
export using ::CD3DX12_RESOURCE_DESC;
export using ::D3D12_CLEAR_VALUE;
export using ::CD3DX12_CLEAR_VALUE;
export using ::D3D12_SUBRESOURCE_DATA;

// =========================================================================================================
// Enums
// =========================================================================================================
// D3D12_HEAP_TYPE
export using ::D3D12_HEAP_TYPE;
export using ::D3D12_HEAP_TYPE_CUSTOM;
export using ::D3D12_HEAP_TYPE_DEFAULT;
export using ::D3D12_HEAP_TYPE_GPU_UPLOAD;
export using ::D3D12_HEAP_TYPE_READBACK;
export using ::D3D12_HEAP_TYPE_UPLOAD;

// D3D12_RESOURCE_DIMENSION
export using ::D3D12_RESOURCE_DIMENSION;
export using ::D3D12_RESOURCE_DIMENSION_BUFFER;
export using ::D3D12_RESOURCE_DIMENSION_TEXTURE1D;
export using ::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
export using ::D3D12_RESOURCE_DIMENSION_TEXTURE3D;
export using ::D3D12_RESOURCE_DIMENSION_UNKNOWN;

// D3D12_TEXTURE_LAYOUT
export using ::D3D12_TEXTURE_LAYOUT;
export using ::D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE;
export using ::D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE;
export using ::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
export using ::D3D12_TEXTURE_LAYOUT_UNKNOWN;

// D3D12_RESOURCE_STATES
export using ::D3D12_RESOURCE_STATES;
export using ::D3D12_RESOURCE_STATE_COMMON;
export using ::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
export using ::D3D12_RESOURCE_STATE_INDEX_BUFFER;
export using ::D3D12_RESOURCE_STATE_RENDER_TARGET;
export using ::D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
export using ::D3D12_RESOURCE_STATE_DEPTH_WRITE;
export using ::D3D12_RESOURCE_STATE_DEPTH_READ;
export using ::D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
export using ::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
export using ::D3D12_RESOURCE_STATE_STREAM_OUT;
export using ::D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
export using ::D3D12_RESOURCE_STATE_COPY_DEST;
export using ::D3D12_RESOURCE_STATE_COPY_SOURCE;
export using ::D3D12_RESOURCE_STATE_RESOLVE_DEST;
export using ::D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
export using ::D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
export using ::D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
export using ::D3D12_RESOURCE_STATE_GENERIC_READ;
export using ::D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
export using ::D3D12_RESOURCE_STATE_PRESENT;
export using ::D3D12_RESOURCE_STATE_PREDICATION;
export using ::D3D12_RESOURCE_STATE_VIDEO_DECODE_READ;
export using ::D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE;
export using ::D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ;
export using ::D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE;
export using ::D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ;
export using ::D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE;

export using ::D3D12_RESOURCE_FLAGS;
export using ::D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;
export using ::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
export using ::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
export using ::D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
export using ::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
export using ::D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
export using ::D3D12_RESOURCE_FLAG_NONE;
export using ::D3D12_RESOURCE_FLAG_RAYTRACING_ACCELERATION_STRUCTURE;
export using ::D3D12_RESOURCE_FLAG_USE_TIGHT_ALIGNMENT;
#pragma endregion

#pragma region Views
// =========================================================================================================
// Views
// =========================================================================================================
export using ::D3D12_SHADER_RESOURCE_VIEW_DESC;
export using ::CD3DX12_SHADER_RESOURCE_VIEW_DESC;
export using ::D3D12_UNORDERED_ACCESS_VIEW_DESC;
export using ::CD3DX12_UNORDERED_ACCESS_VIEW_DESC;
export using ::D3D12_RENDER_TARGET_VIEW_DESC;
export using ::D3D12_DEPTH_STENCIL_VIEW_DESC;

// =========================================================================================================
// Enums
// =========================================================================================================
// D3D12_SRV_DIMENSION
export using ::D3D12_SRV_DIMENSION;
export using ::D3D12_SRV_DIMENSION_BUFFER;
export using ::D3D12_SRV_DIMENSION_BUFFER_BYTE_OFFSET;
export using ::D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
export using ::D3D12_SRV_DIMENSION_TEXTURE1D;
export using ::D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
export using ::D3D12_SRV_DIMENSION_TEXTURE2D;
export using ::D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
export using ::D3D12_SRV_DIMENSION_TEXTURE2DMS;
export using ::D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
export using ::D3D12_SRV_DIMENSION_TEXTURE3D;
export using ::D3D12_SRV_DIMENSION_TEXTURECUBE;
export using ::D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
export using ::D3D12_SRV_DIMENSION_UNKNOWN;

// D3D12_UAV_DIMENSION
export using ::D3D12_UAV_DIMENSION;
export using ::D3D12_UAV_DIMENSION_BUFFER;
export using ::D3D12_UAV_DIMENSION_BUFFER_BYTE_OFFSET;
export using ::D3D12_UAV_DIMENSION_TEXTURE1D;
export using ::D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
export using ::D3D12_UAV_DIMENSION_TEXTURE2D;
export using ::D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
export using ::D3D12_UAV_DIMENSION_TEXTURE2DMS;
export using ::D3D12_UAV_DIMENSION_TEXTURE2DMSARRAY;
export using ::D3D12_UAV_DIMENSION_TEXTURE3D;
export using ::D3D12_UAV_DIMENSION_UNKNOWN;

// D3D12_RTV_DIMENSION
export using ::D3D12_RTV_DIMENSION;
export using ::D3D12_RTV_DIMENSION_BUFFER;
export using ::D3D12_RTV_DIMENSION_TEXTURE1D;
export using ::D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
export using ::D3D12_RTV_DIMENSION_TEXTURE2D;
export using ::D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
export using ::D3D12_RTV_DIMENSION_TEXTURE2DMS;
export using ::D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
export using ::D3D12_RTV_DIMENSION_TEXTURE3D;
export using ::D3D12_RTV_DIMENSION_UNKNOWN;

// D3D12_DSV_DIMENSION
export using ::D3D12_DSV_DIMENSION;
export using ::D3D12_DSV_DIMENSION_TEXTURE1D;
export using ::D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
export using ::D3D12_DSV_DIMENSION_TEXTURE2D;
export using ::D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
export using ::D3D12_DSV_DIMENSION_TEXTURE2DMS;
export using ::D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
export using ::D3D12_DSV_DIMENSION_UNKNOWN;

// D3D12_BUFFER_SRV_FLAGS
export using ::D3D12_BUFFER_SRV_FLAGS;
export using ::D3D12_BUFFER_SRV_FLAG_RAW;
export using ::D3D12_BUFFER_SRV_FLAG_NONE;

// D3D12_BUFFER_UAV_FLAGS
export using ::D3D12_BUFFER_UAV_FLAGS;
export using ::D3D12_BUFFER_UAV_FLAG_RAW;
export using ::D3D12_BUFFER_UAV_FLAG_NONE;
#pragma endregion

#pragma region Enums
// =========================================================================================================
// Enums
// =========================================================================================================
// D3D_FEATURE_LEVEL
export using ::D3D_FEATURE_LEVEL_1_0_CORE;
export using ::D3D_FEATURE_LEVEL_1_0_GENERIC;
export using ::D3D_FEATURE_LEVEL_9_1;
export using ::D3D_FEATURE_LEVEL_9_2;
export using ::D3D_FEATURE_LEVEL_9_3;
export using ::D3D_FEATURE_LEVEL_10_0;
export using ::D3D_FEATURE_LEVEL_10_1;
export using ::D3D_FEATURE_LEVEL_11_0;
export using ::D3D_FEATURE_LEVEL_11_1;
export using ::D3D_FEATURE_LEVEL_12_0;
export using ::D3D_FEATURE_LEVEL_12_1;
export using ::D3D_FEATURE_LEVEL_12_2;

// DXGI_FORMAT
export using ::DXGI_FORMAT;
export using ::DXGI_FORMAT_UNKNOWN;
export using ::DXGI_FORMAT_R32G32B32A32_TYPELESS;
export using ::DXGI_FORMAT_R32G32B32A32_FLOAT;
export using ::DXGI_FORMAT_R32G32B32A32_UINT;
export using ::DXGI_FORMAT_R32G32B32A32_SINT;
export using ::DXGI_FORMAT_R32G32B32_TYPELESS;
export using ::DXGI_FORMAT_R32G32B32_FLOAT;
export using ::DXGI_FORMAT_R32G32B32_UINT;
export using ::DXGI_FORMAT_R32G32B32_SINT;
export using ::DXGI_FORMAT_R16G16B16A16_TYPELESS;
export using ::DXGI_FORMAT_R16G16B16A16_FLOAT;
export using ::DXGI_FORMAT_R16G16B16A16_UNORM;
export using ::DXGI_FORMAT_R16G16B16A16_UINT;
export using ::DXGI_FORMAT_R16G16B16A16_SNORM;
export using ::DXGI_FORMAT_R16G16B16A16_SINT;
export using ::DXGI_FORMAT_R32G32_TYPELESS;
export using ::DXGI_FORMAT_R32G32_FLOAT;
export using ::DXGI_FORMAT_R32G32_UINT;
export using ::DXGI_FORMAT_R32G32_SINT;
export using ::DXGI_FORMAT_R32G8X24_TYPELESS;
export using ::DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
export using ::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
export using ::DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
export using ::DXGI_FORMAT_R10G10B10A2_TYPELESS;
export using ::DXGI_FORMAT_R10G10B10A2_UNORM;
export using ::DXGI_FORMAT_R10G10B10A2_UINT;
export using ::DXGI_FORMAT_R11G11B10_FLOAT;
export using ::DXGI_FORMAT_R8G8B8A8_TYPELESS;
export using ::DXGI_FORMAT_R8G8B8A8_UNORM;
export using ::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
export using ::DXGI_FORMAT_R8G8B8A8_UINT;
export using ::DXGI_FORMAT_R8G8B8A8_SNORM;
export using ::DXGI_FORMAT_R8G8B8A8_SINT;
export using ::DXGI_FORMAT_R16G16_TYPELESS;
export using ::DXGI_FORMAT_R16G16_FLOAT;
export using ::DXGI_FORMAT_R16G16_UNORM;
export using ::DXGI_FORMAT_R16G16_UINT;
export using ::DXGI_FORMAT_R16G16_SNORM;
export using ::DXGI_FORMAT_R16G16_SINT;
export using ::DXGI_FORMAT_R32_TYPELESS;
export using ::DXGI_FORMAT_D32_FLOAT;
export using ::DXGI_FORMAT_R32_FLOAT;
export using ::DXGI_FORMAT_R32_UINT;
export using ::DXGI_FORMAT_R32_SINT;
export using ::DXGI_FORMAT_R24G8_TYPELESS;
export using ::DXGI_FORMAT_D24_UNORM_S8_UINT;
export using ::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
export using ::DXGI_FORMAT_X24_TYPELESS_G8_UINT;
export using ::DXGI_FORMAT_R8G8_TYPELESS;
export using ::DXGI_FORMAT_R8G8_UNORM;
export using ::DXGI_FORMAT_R8G8_UINT;
export using ::DXGI_FORMAT_R8G8_SNORM;
export using ::DXGI_FORMAT_R8G8_SINT;
export using ::DXGI_FORMAT_R16_TYPELESS;
export using ::DXGI_FORMAT_R16_FLOAT;
export using ::DXGI_FORMAT_D16_UNORM;
export using ::DXGI_FORMAT_R16_UNORM;
export using ::DXGI_FORMAT_R16_UINT;
export using ::DXGI_FORMAT_R16_SNORM;
export using ::DXGI_FORMAT_R16_SINT;
export using ::DXGI_FORMAT_R8_TYPELESS;
export using ::DXGI_FORMAT_R8_UNORM;
export using ::DXGI_FORMAT_R8_UINT;
export using ::DXGI_FORMAT_R8_SNORM;
export using ::DXGI_FORMAT_R8_SINT;
export using ::DXGI_FORMAT_A8_UNORM;
export using ::DXGI_FORMAT_R1_UNORM;
export using ::DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
export using ::DXGI_FORMAT_R8G8_B8G8_UNORM;
export using ::DXGI_FORMAT_G8R8_G8B8_UNORM;
export using ::DXGI_FORMAT_BC1_TYPELESS;
export using ::DXGI_FORMAT_BC1_UNORM;
export using ::DXGI_FORMAT_BC1_UNORM_SRGB;
export using ::DXGI_FORMAT_BC2_TYPELESS;
export using ::DXGI_FORMAT_BC2_UNORM;
export using ::DXGI_FORMAT_BC2_UNORM_SRGB;
export using ::DXGI_FORMAT_BC3_TYPELESS;
export using ::DXGI_FORMAT_BC3_UNORM;
export using ::DXGI_FORMAT_BC3_UNORM_SRGB;
export using ::DXGI_FORMAT_BC4_TYPELESS;
export using ::DXGI_FORMAT_BC4_UNORM;
export using ::DXGI_FORMAT_BC4_SNORM;
export using ::DXGI_FORMAT_BC5_TYPELESS;
export using ::DXGI_FORMAT_BC5_UNORM;
export using ::DXGI_FORMAT_BC5_SNORM;
export using ::DXGI_FORMAT_B5G6R5_UNORM;
export using ::DXGI_FORMAT_B5G5R5A1_UNORM;
export using ::DXGI_FORMAT_B8G8R8A8_UNORM;
export using ::DXGI_FORMAT_B8G8R8X8_UNORM;
export using ::DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
export using ::DXGI_FORMAT_B8G8R8A8_TYPELESS;
export using ::DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
export using ::DXGI_FORMAT_B8G8R8X8_TYPELESS;
export using ::DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
export using ::DXGI_FORMAT_BC6H_TYPELESS;
export using ::DXGI_FORMAT_BC6H_UF16;
export using ::DXGI_FORMAT_BC6H_SF16;
export using ::DXGI_FORMAT_BC7_TYPELESS;
export using ::DXGI_FORMAT_BC7_UNORM;
export using ::DXGI_FORMAT_BC7_UNORM_SRGB;
export using ::DXGI_FORMAT_AYUV;
export using ::DXGI_FORMAT_Y410;
export using ::DXGI_FORMAT_Y416;
export using ::DXGI_FORMAT_NV12;
export using ::DXGI_FORMAT_P010;
export using ::DXGI_FORMAT_P016;
export using ::DXGI_FORMAT_420_OPAQUE;
export using ::DXGI_FORMAT_YUY2;
export using ::DXGI_FORMAT_Y210;
export using ::DXGI_FORMAT_Y216;
export using ::DXGI_FORMAT_NV11;
export using ::DXGI_FORMAT_AI44;
export using ::DXGI_FORMAT_IA44;
export using ::DXGI_FORMAT_P8;
export using ::DXGI_FORMAT_A8P8;
export using ::DXGI_FORMAT_B4G4R4A4_UNORM;
export using ::DXGI_FORMAT_P208;
export using ::DXGI_FORMAT_V208;
export using ::DXGI_FORMAT_V408;
export using ::DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE;
export using ::DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE;
export using ::DXGI_FORMAT_FORCE_UINT;
#pragma endregion

#pragma region Misc
// Format support
export using ::D3D12_FEATURE_DATA_FORMAT_SUPPORT;
export using ::D3D12_FORMAT_SUPPORT1_NONE;
export using ::D3D12_FORMAT_SUPPORT2_NONE;
export using ::D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD;
export using ::D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE;
export using ::D3D12_FEATURE_FORMAT_SUPPORT;
export using ::D3D12_FEATURE_DATA_D3D12_OPTIONS;
export using ::D3D12_FEATURE_D3D12_OPTIONS;
#pragma endregion
// NOLINTEND
