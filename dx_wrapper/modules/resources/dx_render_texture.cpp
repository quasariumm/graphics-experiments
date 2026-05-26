module;

module dx_wrapper.resources.dx_render_texture;
import std;
import dx_wrapper.external.directx12;
import dx_wrapper.core.log;

DxRenderTexture::DxRenderTexture(const DxDevice& device, const std::filesystem::path& path, bool generateMips, bool generateSrv)
	: DxTexture{device, path, generateMips, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET},
	  m_rtvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	GenerateDescriptors(device, generateSrv, GetTextureType() == TextureType::DCube);

	// RTV
	m_rtv = m_rtvHeap.GetFirstCpuHandle();
	CreateRenderTargetView(device, m_rtv, 0);
}

DxRenderTexture::DxRenderTexture(const DxDevice& device, const std::byte* data, TextureType type, DXGI_FORMAT format,
								 std::uint32_t width, std::uint32_t height, std::uint32_t depth, bool generateMips,
								 bool generateSrv)
	: DxTexture{device, data, type, format, width, height, depth, generateMips, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET},
	  m_rtvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	GenerateDescriptors(device, generateSrv, GetTextureType() == TextureType::DCube);

	// RTV
	m_rtv = m_rtvHeap.GetFirstCpuHandle();
	CreateRenderTargetView(device, m_rtv, 0);
}

DxRenderTexture::DxRenderTexture(const DxDevice& device, TextureType type, DXGI_FORMAT format, std::uint32_t width,
								 std::uint32_t height, std::uint32_t depth, bool allocateMips, bool generateSrv)
	: DxTexture{device, type, format, width, height, depth, allocateMips, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET},
	  m_rtvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	GenerateDescriptors(device, generateSrv, GetTextureType() == TextureType::DCube);

	// RTV
	m_rtv = m_rtvHeap.GetFirstCpuHandle();
	CreateRenderTargetView(device, m_rtv, 0);
}

DxRenderTexture::DxRenderTexture(const DxDevice& device, const std::byte* data, std::size_t size, bool generateMips,
								 bool generateSrv)
	: DxTexture{device, data, size, generateMips, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET},
	  m_rtvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	GenerateDescriptors(device, generateSrv, GetTextureType() == TextureType::DCube);

	// RTV
	m_rtv = m_rtvHeap.GetFirstCpuHandle();
	CreateRenderTargetView(device, m_rtv, 0);
}

void DxRenderTexture::CreateRenderTargetView(const DxDevice& device, D3D12_CPU_DESCRIPTOR_HANDLE handle,
											 std::uint32_t mipLevel) const
{
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = GetFormat();

	switch (GetTextureType())
	{
	case TextureType::D1:
		rtvDesc.ViewDimension	   = D3D12_RTV_DIMENSION_TEXTURE1D;
		rtvDesc.Texture1D.MipSlice = mipLevel;
		break;
	case TextureType::D1Array:
		rtvDesc.ViewDimension				   = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
		rtvDesc.Texture1DArray.MipSlice		   = mipLevel;
		rtvDesc.Texture1DArray.FirstArraySlice = 0;
		rtvDesc.Texture1DArray.ArraySize	   = 1;
		break;
	case TextureType::D2:
		rtvDesc.ViewDimension	   = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		break;
	case TextureType::D2Array:
	case TextureType::DCube:
		rtvDesc.ViewDimension				   = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.MipSlice		   = mipLevel;
		rtvDesc.Texture2DArray.FirstArraySlice = 0;
		rtvDesc.Texture2DArray.ArraySize	   = (GetTextureType() == TextureType::DCube) ? 6 : 1;
		break;
	case TextureType::D3:
		Log::Critical("3D depth stencil textures are not supported in D3D12");
		return;
	default:
		Log::Critical("Invalid texture type");
		return;
	}

	device->CreateRenderTargetView(DxTexture::GetResource(), &rtvDesc, handle);
}
