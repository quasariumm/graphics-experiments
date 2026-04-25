module;

#include <DirectXHelpers.h>
#include <filesystem>

module dx_wrapper.resources.dx_depth_render_texture;
import dx_wrapper.core.log;

DxDepthRenderTexture::DxDepthRenderTexture(const DxDevice& device, const std::filesystem::path& path, const bool generateMips,
										   const bool generateSrv)
	: DxTexture{device, path, generateMips, true},
	  m_dsvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	GenerateDescriptors(device, generateSrv, GetTextureType() == TextureType::DCube);
	const D3D12_RESOURCE_DESC dsvDesc = DxTexture::GetResource()->GetDesc();
	CreateDsv(device, GetTextureType(), GetFormat(), dsvDesc.DepthOrArraySize);
}

DxDepthRenderTexture::DxDepthRenderTexture(const DxDevice& device, const void* data, const TextureType type,
										   const DXGI_FORMAT format, const uint32_t width, const uint32_t height,
										   const uint32_t depth, const bool generateMips, const bool generateSrv)
	: DxTexture{device, data, type, format, width, height, depth, generateMips, true},
	  m_dsvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	GenerateDescriptors(device, generateSrv, GetTextureType() == TextureType::DCube);
	CreateDsv(device, type, format, depth);
}

DxDepthRenderTexture::DxDepthRenderTexture(const DxDevice& device, const TextureType type, const DXGI_FORMAT format,
										   const uint32_t width, const uint32_t height, const uint32_t depth,
										   const bool allocateMips, const bool generateSrv)
	: DxTexture{device, type, format, width, height, depth, allocateMips, true},
	  m_dsvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	GenerateDescriptors(device, generateSrv, GetTextureType() == TextureType::DCube);
	CreateDsv(device, type, format, depth);
}

DxDepthRenderTexture::DxDepthRenderTexture(const DxDevice& device, const void* data, const size_t size, const bool generateMips,
										   const bool generateSrv)
	: DxTexture{device, data, size, generateMips, true},
	  m_dsvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	GenerateDescriptors(device, generateSrv, GetTextureType() == TextureType::DCube);
	const D3D12_RESOURCE_DESC dsvDesc = DxTexture::GetResource()->GetDesc();
	CreateDsv(device, GetTextureType(), GetFormat(), dsvDesc.DepthOrArraySize);
}

void DxDepthRenderTexture::CreateDsv(const DxDevice& device, TextureType type, DXGI_FORMAT format, uint32_t depth)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format						  = format;

	switch (type)
	{
	case TextureType::D1:
		dsvDesc.ViewDimension	   = D3D12_DSV_DIMENSION_TEXTURE1D;
		dsvDesc.Texture1D.MipSlice = 0;
		break;
	case TextureType::D1Array:
		dsvDesc.ViewDimension				   = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
		dsvDesc.Texture1DArray.MipSlice		   = 0;
		dsvDesc.Texture1DArray.FirstArraySlice = 0;
		dsvDesc.Texture1DArray.ArraySize	   = depth;
		break;
	case TextureType::D2:
		dsvDesc.ViewDimension	   = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		break;
	case TextureType::D2Array:
	case TextureType::DCube:
		dsvDesc.ViewDimension				   = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.MipSlice		   = 0;
		dsvDesc.Texture2DArray.FirstArraySlice = 0;
		dsvDesc.Texture2DArray.ArraySize	   = (type == TextureType::DCube) ? 6 : depth;
		break;
	case TextureType::D3:
		Log::Critical("3D depth stencil textures are not supported in D3D12");
		return;
	default:
		Log::Critical("Invalid texture type");
		return;
	}

	m_dsv = m_dsvHeap.GetFirstCpuHandle();
	device->CreateDepthStencilView(DxTexture::GetResource(), &dsvDesc, m_dsv);
}
