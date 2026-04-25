module;

#include <d3dx12.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <wrl/client.h>

export module dx_wrapper.core.dx_common;
import dx_wrapper.core.log;

export using Microsoft::WRL::ComPtr;

export inline constexpr uint32_t max_shader_descriptors = 16384;

export void CheckHR(HRESULT hr)
{
	if (FAILED(hr))
	{
		CHAR  errMsgBuf[256];
		DWORD errMsgLen = 0;

		errMsgLen = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								   nullptr,
								   hr,
								   0,
								   errMsgBuf,
								   sizeof(errMsgBuf),
								   nullptr);

		if (errMsgLen != 0)
		{
			const std::string errorStr(errMsgBuf);
			Log::Critical("{}", errorStr);
		}
		else
		{
			Log::Critical("A DirectX 12 error has occured");
		}
	}
}

export std::vector<char> ReadFileBinary(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path))
	{
		Log::Error("File {} nor found", path.string());
		return {};
	}
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	auto		  size = file.tellg();
	file.seekg(0);
	std::vector<char> data(size);
	file.read(data.data(), size);
	return data;
}

export inline uint8_t GetChannelCount(const DXGI_FORMAT fmt)
{
	switch (fmt)
	{
	// ---------------------------------------------------------
	// 8‑bit per component
	// ---------------------------------------------------------
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SINT:
		return 1;

	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SINT:
		return 2;

	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return 4;

	// ---------------------------------------------------------
	// 16‑bit per component (or mixed)
	// ---------------------------------------------------------
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_D16_UNORM: // depth treated as 1 channel
		return 1;

	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SINT:
		return 2;

	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return 4;

	// ---------------------------------------------------------
	// 32‑bit per component (or mixed)
	// ---------------------------------------------------------
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
		return 1;

	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
		return 2;

	case DXGI_FORMAT_R32G32B32_FLOAT:
		return 3; // only 3‑component float format

	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 4;

	// ---------------------------------------------------------
	// Packed formats
	// ---------------------------------------------------------
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
		return 4;

	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		return 4;

	case DXGI_FORMAT_R11G11B10_FLOAT:
		return 3;

	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		return 3;

	// ---------------------------------------------------------
	// Depth‑stencil (non‑color)
	// ---------------------------------------------------------
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		return 0;

	// ---------------------------------------------------------
	// Block‑compressed formats (cannot be expressed as simple channels)
	// ---------------------------------------------------------
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return 0;

	// ---------------------------------------------------------
	// Typeless formats – map to the most common concrete layout
	// ---------------------------------------------------------
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		return 4;
	case DXGI_FORMAT_R32G32B32_TYPELESS:
		return 3;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		return 4;
	case DXGI_FORMAT_R32G32_TYPELESS:
		return 2;
	case DXGI_FORMAT_R32_TYPELESS:
		return 1;
	case DXGI_FORMAT_R16G16_TYPELESS:
		return 2;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		return 4;
	case DXGI_FORMAT_R8G8_TYPELESS:
		return 2;
	case DXGI_FORMAT_R16_TYPELESS:
		return 1;
	case DXGI_FORMAT_R8_TYPELESS:
		return 1;
	case DXGI_FORMAT_R24G8_TYPELESS:
		return 0; // depth‑stencil typeless
	case DXGI_FORMAT_R32G8X24_TYPELESS:
		return 0; // depth‑stencil typeless

	// ---------------------------------------------------------
	// Fallback – any format not explicitly listed
	// ---------------------------------------------------------
	default:
		return 0;
	}
}
