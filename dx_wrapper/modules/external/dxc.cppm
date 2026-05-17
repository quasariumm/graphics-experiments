// NOLINTBEGIN
module;

#include <d3d12.h>
#include <dxcapi.h>
#include <wrl/client.h>

export module dx_wrapper.external.dxc;
export import dx_wrapper.external.directx12;

export using ::DxcCreateInstance;
export using ::DxcCreateInstance2;
export using ::DxcCreateInstanceProc;
export using ::DxcCreateInstance2Proc;

export using ::IDxcCompiler3;
export using ::IDxcUtils;
export using ::IDxcValidator;
export using ::IDxcValidator2;

export namespace dxc
{
inline constexpr CLSID CLSID_DxcCompiler  = {0x73e22d93, 0xe6ce, 0x47f3, {0xb5, 0xbf, 0xf0, 0x66, 0x4f, 0x39, 0xc1, 0xb0}};
inline constexpr CLSID CLSID_DxcUtils	  = {0x6245d6af, 0x66e0, 0x48fd, {0x80, 0xb4, 0x4d, 0x27, 0x17, 0x96, 0x74, 0x8c}};
inline constexpr CLSID CLSID_DxcValidator = {0x8ca3e215, 0xf728, 0x4cf3, {0x8c, 0xdd, 0x88, 0xaf, 0x91, 0x75, 0x87, 0xa1}};
} // namespace dxc

export using ::IDxcBlob;
export using ::IDxcBlobEncoding;
export using ::IDxcBlobUtf8;
export using IDxcBlobUtf16 = ::IDxcBlobWide;

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

export constexpr UINT32 DxcValidatorFlagsDefault		   = ::DxcValidatorFlags_Default;
export constexpr UINT32 DxcValidatorFlagsInPlaceEdit	   = ::DxcValidatorFlags_InPlaceEdit;
export constexpr UINT32 DxcValidatorFlagsModuleOnly		   = ::DxcValidatorFlags_ModuleOnly;
export constexpr UINT32 DxcValidatorFlagsRootSignatureOnly = ::DxcValidatorFlags_RootSignatureOnly;
export constexpr UINT32 DxcValidatorFlagsValidMask		   = ::DxcValidatorFlags_ValidMask;

static constexpr int DXC_CP_ACP_val = DXC_CP_ACP;
#undef DXC_CP_ACP
export constexpr int DXC_CP_ACP = DXC_CP_ACP_val;

#define REGISTER_IID(Type, ...) \
	inline constexpr GUID IID_##Type = {__VA_ARGS__};\
	export FORCEINLINE const GUID& GetIID(const Microsoft::WRL::ComPtr<Type>&) { return IID_##Type; }\
	export FORCEINLINE const GUID& GetIID(Type**) { return IID_##Type; }

REGISTER_IID(IDxcCompiler3, 0x228b4687, 0x5a6a, 0x4730, {0x90, 0x0c, 0x97, 0x02, 0xb2, 0x20, 0x3f, 0x54});
REGISTER_IID(IDxcUtils, 0x4605c4cb, 0x2019, 0x492a, {0xad,0xa4,0x65,0xf2,0x0b,0xb7,0xd6,0x7f})
REGISTER_IID(IDxcValidator, 0xa6e82bd2, 0x1fd7, 0x4826, {0x98, 0x11, 0x28, 0x57, 0xe7, 0x97, 0xf4, 0x9a})
REGISTER_IID(IDxcValidator2, 0x458e1fd1, 0xb1b2, 0x4750, {0xa6,0xe1,0x9c,0x10,0xf0,0x3b,0xed,0x92})
REGISTER_IID(IDxcBlob, 0x8ba5fb08, 0x5195, 0x40e2, {0xac, 0x58, 0x0d, 0x98, 0x9c, 0x3a, 0x01, 0x02})
REGISTER_IID(IDxcBlobEncoding, 0x7241d424, 0x2646, 0x4191, {0x97, 0xc0, 0x98, 0xe9, 0x6e, 0x42, 0xfc, 0x68})
REGISTER_IID(IDxcBlobUtf8, 0x3da636c9, 0xba71, 0x4024, {0xa3, 0x01, 0x30, 0xcb, 0xf1, 0x25, 0x30, 0x5b})
REGISTER_IID(IDxcBlobWide, 0xa3f84eab, 0x0faa, 0x497e, {0xa3, 0x9c, 0xee, 0x6e, 0xd6, 0x0b, 0x2d, 0x84})
REGISTER_IID(IDxcIncludeHandler, 0x7f61fc7d, 0x950d, 0x467f, {0xb3, 0xe3, 0x3c, 0x02, 0xfb, 0x49, 0x18, 0x7c})
REGISTER_IID(IDxcResult, 0x58346cda, 0xdde7, 0x4497, {0x94, 0x61, 0x6f, 0x87, 0xaf, 0x5e, 0x06, 0x59})
REGISTER_IID(IDxcOperationResult, 0xcedb484a, 0xd4e9, 0x445a, {0xb9, 0x91, 0xca, 0x21, 0xca, 0x15, 0x7d, 0xc2})
// NOLINTEND
