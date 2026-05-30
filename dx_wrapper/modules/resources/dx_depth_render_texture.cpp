module;

module dx_wrapper.resources.dx_depth_render_texture;
import std;
import dx_wrapper.external.directx12;
import dx_wrapper.core.log;

DxDepthRenderTexture::DxDepthRenderTexture(const DxDevice& device, const std::filesystem::path& path, const bool generateMips,
										   const bool generateSrv)
	: DxTexture{device, path, generateMips, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL},
	  m_dsvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	if (generateSrv)
		UpdateOrCreateSrv(device, GetTextureType() == TextureType::DCube);
#ifdef _MSC_VER
	const D3D12_RESOURCE_DESC dsvDesc = DxTexture::GetResource()->GetDesc();
#else
	D3D12_RESOURCE_DESC dsvDesc;
	DxTexture::GetResource()->GetDesc(&dsvDesc);
#endif
	UpdateOrCreateDsv(device, GetTextureType(), GetFormat(), dsvDesc.DepthOrArraySize);
}

DxDepthRenderTexture::DxDepthRenderTexture(const DxDevice& device, const std::byte* data, const TextureType type,
										   const DXGI_FORMAT format, const std::uint32_t width, const std::uint32_t height,
										   const std::uint32_t depth, const bool generateMips, const bool generateSrv)
	: DxTexture{device, data, type, format, width, height, depth, generateMips, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL},
	  m_dsvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	if (generateSrv)
		UpdateOrCreateSrv(device, GetTextureType() == TextureType::DCube);
	UpdateOrCreateDsv(device, type, format, depth);
}

DxDepthRenderTexture::DxDepthRenderTexture(const DxDevice& device, const TextureType type, const DXGI_FORMAT format,
										   const std::uint32_t width, const std::uint32_t height, const std::uint32_t depth,
										   const bool allocateMips, const bool generateSrv)
	: DxTexture{device, type, format, width, height, depth, allocateMips, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL},
	  m_dsvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	if (generateSrv)
		UpdateOrCreateSrv(device, GetTextureType() == TextureType::DCube);
	UpdateOrCreateDsv(device, type, format, depth);
}

DxDepthRenderTexture::DxDepthRenderTexture(const DxDevice& device, const std::byte* data, const std::size_t size,
										   const bool generateMips, const bool generateSrv)
	: DxTexture{device, data, size, generateMips, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL},
	  m_dsvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	if (generateSrv)
		UpdateOrCreateSrv(device, GetTextureType() == TextureType::DCube);
#ifdef _MSC_VER
	const D3D12_RESOURCE_DESC dsvDesc = DxTexture::GetResource()->GetDesc();
#else
	D3D12_RESOURCE_DESC dsvDesc;
	DxTexture::GetResource()->GetDesc(&dsvDesc);
#endif
	UpdateOrCreateDsv(device, GetTextureType(), GetFormat(), dsvDesc.DepthOrArraySize);
}

void DxDepthRenderTexture::UpdateOrCreateDsv(const DxDevice& device, TextureType type, DXGI_FORMAT format, std::uint32_t depth, bool update)
{
	if (!update)
	{
		m_dsvDesc.Format = format;

		switch (type)
		{
		case TextureType::D1:
			m_dsvDesc.ViewDimension		 = D3D12_DSV_DIMENSION_TEXTURE1D;
			m_dsvDesc.Texture1D.MipSlice = 0;
			break;
		case TextureType::D1Array:
			m_dsvDesc.ViewDimension					 = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
			m_dsvDesc.Texture1DArray.MipSlice		 = 0;
			m_dsvDesc.Texture1DArray.FirstArraySlice = 0;
			m_dsvDesc.Texture1DArray.ArraySize		 = depth;
			break;
		case TextureType::D2:
			m_dsvDesc.ViewDimension		 = D3D12_DSV_DIMENSION_TEXTURE2D;
			m_dsvDesc.Texture2D.MipSlice = 0;
			break;
		case TextureType::D2Array:
		case TextureType::DCube:
			m_dsvDesc.ViewDimension					 = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			m_dsvDesc.Texture2DArray.MipSlice		 = 0;
			m_dsvDesc.Texture2DArray.FirstArraySlice = 0;
			m_dsvDesc.Texture2DArray.ArraySize		 = (type == TextureType::DCube) ? 6 : depth;
			break;
		case TextureType::D3:
			Log::Critical("3D depth stencil textures are not supported in D3D12");
			return;
		default:
			Log::Critical("Invalid texture type");
			return;
		}
	}

	m_dsv = m_dsvHeap.GetFirstCpuHandle();
	device->CreateDepthStencilView(DxTexture::GetResource(), &m_dsvDesc, m_dsv);
}

void DxDepthRenderTexture::DerivedUpdateDescriptors(const DxDevice& device)
{
	UpdateOrCreateDsv(device, TextureType::Invalid, DXGI_FORMAT_UNKNOWN, 0, true);
}
