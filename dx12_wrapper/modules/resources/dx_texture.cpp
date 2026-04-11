module;

#include <d3d12.h>
#include <filesystem>
#include <wincodec.h>

#include <BufferHelpers.h>
#include <DDSTextureLoader.h>
#include <DirectXHelpers.h>
#include <WICTextureLoader.h>
#include <../Src/LoaderHelpers.h>

module dx_wrapper.resources.dx_texture;
import dx_wrapper.rendering.dx_descriptor_pile;
import dx_wrapper.core;

TextureType DxToTextureType(const D3D12_RESOURCE_DIMENSION dim, const UINT depthOrArraySize)
{
	switch (dim)
	{
	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
		return TextureType::D1;
	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		return depthOrArraySize > 1 ? TextureType::D2Array : TextureType::D2;
	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		return depthOrArraySize > 1 ? TextureType::D3Array : TextureType::D3;
	default:
		return TextureType::Invalid;
	}
}

DxTexture::DxTexture(const DxDevice& device, const std::filesystem::path& path, const bool generateMips, const bool generateUav)
{
	m_device = &device;

	bool isCubemap = false;
	device.GetResourceUpload().Begin();

	if ((path.extension() == ".dds") || (path.extension() == ".dxg"))
	{
		CheckHR(DirectX::CreateDDSTextureFromFileEx(
				*device,
				device.GetResourceUpload(),
				path.c_str(),
				0,
				generateUav ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE,
				generateMips ? DirectX::DDS_LOADER_MIP_AUTOGEN : DirectX::DDS_LOADER_IGNORE_MIPS,
				&m_resource,
				nullptr,
				&isCubemap));
	}
	else
	{
		CheckHR(DirectX::CreateWICTextureFromFileEx(
				*device,
				device.GetResourceUpload(),
				path.c_str(),
				0,
				generateUav ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE,
				generateMips ? DirectX::WIC_LOADER_MIP_AUTOGEN : DirectX::WIC_LOADER_DEFAULT,
				&m_resource));
	}

	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	SetState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	const D3D12_RESOURCE_DESC desc = m_resource->GetDesc();
	m_textureType				   = isCubemap ? TextureType::DCube : DxToTextureType(desc.Dimension, desc.DepthOrArraySize);

	GenerateDescriptors(device, generateUav, isCubemap);
}

DxTexture::DxTexture(const DxDevice& device, const void* data, const TextureType type, const uint32_t width,
					 const uint32_t height, const DXGI_FORMAT format, const bool generateMips, const bool generateUav)
{
	m_textureType = type;

	device.GetResourceUpload().Begin();

	const size_t bytesPerPixel = DirectX::LoaderHelpers::BitsPerPixel(format) >> 3;

	D3D12_SUBRESOURCE_DATA initData{};
	initData.pData		= data;
	initData.RowPitch	= width * bytesPerPixel;
	initData.SlicePitch = width * height * bytesPerPixel;

	CheckHR(DirectX::CreateTextureFromMemory(*device,
											 device.GetResourceUpload(),
											 width,
											 height,
											 format,
											 initData,
											 &m_resource,
											 generateMips));

	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	SetState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	GenerateDescriptors(device, generateUav, type == TextureType::DCube);
}

DxTexture::DxTexture(const DxDevice& device, const void* data, const size_t size, const bool generateMips,
					 const bool generateUav)
{
	bool isDds = size >= 4 && *static_cast<const uint32_t*>(data) == 0x20534444;

	bool isCubemap = false;
	device.GetResourceUpload().Begin();

	if (isDds)
	{
		CheckHR(DirectX::CreateDDSTextureFromMemoryEx(
				*device,
				device.GetResourceUpload(),
				static_cast<const std::byte*>(data),
				size,
				0,
				generateUav ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE,
				generateMips ? DirectX::DDS_LOADER_MIP_AUTOGEN : DirectX::DDS_LOADER_IGNORE_MIPS,
				&m_resource,
				nullptr,
				&isCubemap));
	}
	else
	{
		CheckHR(DirectX::CreateWICTextureFromMemoryEx(
				*device,
				device.GetResourceUpload(),
				static_cast<const std::byte*>(data),
				size,
				0,
				generateUav ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE,
				generateMips ? DirectX::WIC_LOADER_MIP_AUTOGEN : DirectX::WIC_LOADER_DEFAULT,
				&m_resource));
	}

	device.GetResourceUpload().End(device.GetDXDirectComQueue());
	
	SetState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	
	const D3D12_RESOURCE_DESC desc = m_resource->GetDesc();
	m_textureType				   = isCubemap ? TextureType::DCube : DxToTextureType(desc.Dimension, desc.DepthOrArraySize);

	GenerateDescriptors(device, generateUav, isCubemap);
}

DxTexture::~DxTexture()
{
	if (m_device)
	{
		m_device->GetShaderDescriptorPile().Free(m_srvHeapIndex, 1);
		m_device->GetShaderDescriptorPile().Free(m_uavHeapIndex, 1);
	}
}

ID3D12Resource* DxTexture::GetResource() const { return m_resource.Get(); }
ID3D12Resource* DxTexture::operator*() const { return m_resource.Get(); }
ID3D12Resource* DxTexture::operator->() const { return m_resource.Get(); }
int32_t			DxTexture::GetSrvHeapIndex() const { return m_srvHeapIndex; }
int32_t			DxTexture::GetUavHeapIndex() const { return m_uavHeapIndex; }
bool			DxTexture::IsValid() const { return m_resource.Get() != nullptr && m_textureType != TextureType::Invalid; }

void DxTexture::GenerateDescriptors(const DxDevice& device, const bool generateUav, const bool isCubemap)
{
	// Generate descriptors
	auto& descriptorPile = device.GetShaderDescriptorPile();

	if (generateUav)
	{
		int32_t start{0}, end{0};
		descriptorPile.AllocateRange(2, start, end);
		m_srvHeapIndex = start;
		m_uavHeapIndex = end;
		
		const auto srv = descriptorPile.GetCpuHandle(m_srvHeapIndex);
		DirectX::CreateShaderResourceView(*device, m_resource.Get(), srv, isCubemap);

		const auto uav = descriptorPile.GetCpuHandle(m_uavHeapIndex);
		DirectX::CreateUnorderedAccessView(*device, m_resource.Get(), uav, 0);
	}
	else
	{
		m_srvHeapIndex = descriptorPile.Allocate();

		const auto srv = descriptorPile.GetCpuHandle(m_srvHeapIndex);
		DirectX::CreateShaderResourceView(*device, m_resource.Get(), srv, isCubemap);
	}
}
