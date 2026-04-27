// NOLINTBEGIN
module;

#include <d3d12.h>
#include <dxcapi.h>

export module dx_wrapper.external.dxc;

export using ::DxcCreateInstance;
export using ::DxcCreateInstance2;
export using ::DxcCreateInstanceProc;
export using ::DxcCreateInstance2Proc;

export using ::IDxcCompiler;
export using ::IDxcCompiler2;
export using ::IDxcCompiler3;
export using ::IDxcUtils;
export using ::IDxcValidator;
export using ::IDxcValidator2;
export using ::CLSID_DxcCompiler;
export using ::CLSID_DxcUtils;
export using ::CLSID_DxcValidator;

export using ::IDxcBlob;
export using ::IDxcBlobEncoding;
export using ::IDxcBlobUtf8;
export using ::IDxcBlobUtf16;

export using ::IDxcIncludeHandler;
export using ::IDxcResult;
export using ::IDxcOperationResult;

export using ::DxcBuffer;

export using ::DXC_OUT_KIND;
export using ::DXC_OUT_DISASSEMBLY;
export using ::DXC_OUT_ERRORS;
export using ::DXC_OUT_EXTRA_OUTPUTS;
export using ::DXC_OUT_FORCE_DWORD;
export using ::DXC_OUT_HLSL;
export using ::DXC_OUT_NONE;
export using ::DXC_OUT_OBJECT;
export using ::DXC_OUT_PDB;
export using ::DXC_OUT_REFLECTION;
export using ::DXC_OUT_REMARKS;
export using ::DXC_OUT_ROOT_SIGNATURE;
export using ::DXC_OUT_ROOT_SIGNATURE;
export using ::DXC_OUT_SHADER_HASH;
export using ::DXC_OUT_TEXT;
export using ::DXC_OUT_TIME_REPORT;
export using ::DXC_OUT_TIME_TRACE;

export constexpr UINT32 DxcValidatorFlagsDefault = ::DxcValidatorFlags_Default;
export constexpr UINT32 DxcValidatorFlagsInPlaceEdit = ::DxcValidatorFlags_InPlaceEdit;
export constexpr UINT32 DxcValidatorFlagsModuleOnly = ::DxcValidatorFlags_ModuleOnly;
export constexpr UINT32 DxcValidatorFlagsRootSignatureOnly = ::DxcValidatorFlags_RootSignatureOnly;
export constexpr UINT32 DxcValidatorFlagsValidMask = ::DxcValidatorFlags_ValidMask;

static constexpr int DXC_CP_ACP_val = DXC_CP_ACP;
#undef DXC_CP_ACP
export constexpr int DXC_CP_ACP = DXC_CP_ACP_val;
// NOLINTEND
