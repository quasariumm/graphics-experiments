module;

#include <DirectXHelpers.h>

module dx_wrapper.resources.dx_rendertexture;

DxRenderTexture::DxRenderTexture(const DxDevice& device, const std::filesystem::path& path, bool generateMips, bool generateSrv)
	: DxTexture{device, path, generateMips, true},
	  m_rtvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	GenerateDescriptors(device, generateSrv, GetTextureType() == TextureType::DCube);

	// RTV
	m_rtv = m_rtvHeap.GetFirstCpuHandle();
	DirectX::CreateRenderTargetView(*device, DxTexture::GetResource(), m_rtv, 0);
}

DxRenderTexture::DxRenderTexture(const DxDevice& device, const void* data, TextureType type, DXGI_FORMAT format, uint32_t width,
								 uint32_t height, uint32_t depth, bool generateMips, bool generateSrv)
	: DxTexture{device, data, type, format, width, height, depth, generateMips, true},
	  m_rtvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	GenerateDescriptors(device, generateSrv, GetTextureType() == TextureType::DCube);

	// RTV
	m_rtv = m_rtvHeap.GetFirstCpuHandle();
	DirectX::CreateRenderTargetView(*device, DxTexture::GetResource(), m_rtv, 0);
}

DxRenderTexture::DxRenderTexture(const DxDevice& device, TextureType type, DXGI_FORMAT format, uint32_t width, uint32_t height,
								 uint32_t depth, bool allocateMips, bool generateSrv)
	: DxTexture{device, type, format, width, height, depth, allocateMips, true},
	  m_rtvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	GenerateDescriptors(device, generateSrv, GetTextureType() == TextureType::DCube);

	// RTV
	m_rtv = m_rtvHeap.GetFirstCpuHandle();
	DirectX::CreateRenderTargetView(*device, DxTexture::GetResource(), m_rtv, 0);
}

DxRenderTexture::DxRenderTexture(const DxDevice& device, const void* data, size_t size, bool generateMips, bool generateSrv)
	: DxTexture{device, data, size, generateMips, true},
	  m_rtvHeap{*device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1}
{
	GenerateDescriptors(device, generateSrv, GetTextureType() == TextureType::DCube);

	// RTV
	m_rtv = m_rtvHeap.GetFirstCpuHandle();
	DirectX::CreateRenderTargetView(*device, DxTexture::GetResource(), m_rtv, 0);
}
