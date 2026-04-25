module;

#include <d3d12.h>
#include <d3dx12.h>
#include <filesystem>
#include <wincodec.h>

#include <../Src/LoaderHelpers.h>
#include <BufferHelpers.h>
#include <DDSTextureLoader.h>
#include <DirectXHelpers.h>
#include <WICTextureLoader.h>

module dx_wrapper.resources.dx_texture;
import dx_wrapper.rendering.dx_descriptor_pile;
import dx_wrapper.core;

TextureType DxToTextureType(const D3D12_RESOURCE_DIMENSION dim, const UINT depthOrArraySize)
{
	switch (dim)
	{
	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
		return depthOrArraySize > 1 ? TextureType::D1Array : TextureType::D1;
	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		return depthOrArraySize > 1 ? TextureType::D2Array : TextureType::D2;
	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		return TextureType::D3;
	default:
		return TextureType::Invalid;
	}
}

D3D12_RESOURCE_DIMENSION TextureTypeToDx(const TextureType type)
{
	switch (type)
	{
	case TextureType::Invalid:
		return D3D12_RESOURCE_DIMENSION_UNKNOWN;
	case TextureType::D1:
	case TextureType::D1Array:
		return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
	case TextureType::D2:
	case TextureType::D2Array:
	case TextureType::DCube:
		return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	case TextureType::D3:
		return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	}
	return D3D12_RESOURCE_DIMENSION_UNKNOWN;
}

DxTexture::DxTexture(const DxDevice& device, const std::filesystem::path& path, const bool generateMips, const bool _internal)
{
	m_device = &device;

	bool isCubemap = false;
	device.GetResourceUpload().Begin();

	if ((path.extension() == ".dds") || (path.extension() == ".dxg"))
	{
		CheckHR(DirectX::CreateDDSTextureFromFileEx(*device,
													device.GetResourceUpload(),
													path.c_str(),
													0,
													D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
													generateMips ? DirectX::DDS_LOADER_MIP_AUTOGEN
																 : DirectX::DDS_LOADER_IGNORE_MIPS,
													&m_resource,
													nullptr,
													&isCubemap));
	}
	else
	{
		CheckHR(DirectX::CreateWICTextureFromFileEx(*device,
													device.GetResourceUpload(),
													path.c_str(),
													0,
													D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
													generateMips ? DirectX::WIC_LOADER_MIP_AUTOGEN
																 : DirectX::WIC_LOADER_DEFAULT,
													&m_resource));
	}

	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	SetState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	const D3D12_RESOURCE_DESC desc = m_resource->GetDesc();
	m_textureType				   = isCubemap ? TextureType::DCube : DxToTextureType(desc.Dimension, desc.DepthOrArraySize);

	if (!_internal)
		GenerateDescriptors(device, true, isCubemap);
}

DxTexture::DxTexture(const DxDevice& device, const void* data, const TextureType type, const DXGI_FORMAT format,
					 const uint32_t width, const uint32_t height, const uint32_t depth, bool generateMips, const bool _internal)
{
	m_textureType = type;

	device.GetResourceUpload().Begin();

	uint16_t mipCount = 1;
	if (generateMips)
	{
		generateMips = device.GetResourceUpload().IsSupportedForGenerateMips(format);
		if (generateMips)
			mipCount = static_cast<uint16_t>(DirectX::LoaderHelpers::CountMips(width, height));
	}

	const size_t bytesPerPixel = DirectX::LoaderHelpers::BitsPerPixel(format) >> 3;

	D3D12_RESOURCE_DESC texDesc{};
	texDesc.Dimension		 = TextureTypeToDx(type);
	texDesc.Width			 = width;
	texDesc.Height			 = height;
	texDesc.DepthOrArraySize = depth;
	texDesc.Format			 = format;
	texDesc.Flags			 = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	texDesc.Layout			 = D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE;
	texDesc.MipLevels		 = mipCount;
	texDesc.Alignment		 = 0;

	const CD3DX12_HEAP_PROPERTIES heapProp{D3D12_HEAP_TYPE_DEFAULT};

	CheckHR(device->CreateCommittedResource(&heapProp,
											D3D12_HEAP_FLAG_NONE,
											&texDesc,
											D3D12_RESOURCE_STATE_COMMON,
											nullptr,
											IID_PPV_ARGS(&m_resource)));

	D3D12_SUBRESOURCE_DATA initData{};
	initData.pData		= data;
	initData.RowPitch	= width * bytesPerPixel;
	initData.SlicePitch = width * height * bytesPerPixel;

	device.GetResourceUpload().Upload(m_resource.Get(), 0, &initData, 1);

	if (generateMips)
		device.GetResourceUpload().GenerateMips(m_resource.Get());

	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	SetState(D3D12_RESOURCE_STATE_COMMON);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	if (!_internal)
		GenerateDescriptors(device, true, type == TextureType::DCube);
}

DxTexture::DxTexture(const DxDevice& device, TextureType type, DXGI_FORMAT format, uint32_t width, uint32_t height,
					 uint32_t depth, bool allocateMips, const bool _internal)
{
	D3D12_RESOURCE_DESC texDesc{};
	texDesc.Dimension		 = TextureTypeToDx(type);
	texDesc.Width			 = width;
	texDesc.Height			 = height;
	texDesc.DepthOrArraySize = depth;
	texDesc.Format			 = format;
	texDesc.Flags			 = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	texDesc.Layout			 = D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE;
	texDesc.MipLevels		 = allocateMips ? DirectX::LoaderHelpers::CountMips(width, height) : 1;
	texDesc.Alignment		 = 0;

	const CD3DX12_HEAP_PROPERTIES heapProp{D3D12_HEAP_TYPE_DEFAULT};

	CheckHR(device->CreateCommittedResource(&heapProp,
											D3D12_HEAP_FLAG_NONE,
											&texDesc,
											D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
											nullptr,
											IID_PPV_ARGS(&m_resource)));

	if (!_internal)
		GenerateDescriptors(device, true, type == TextureType::DCube);
}

DxTexture::DxTexture(const DxDevice& device, const void* data, const size_t size, const bool generateMips, const bool _internal)
{
	bool isDds = size >= 4 && *static_cast<const uint32_t*>(data) == 0x20534444;

	bool isCubemap = false;
	device.GetResourceUpload().Begin();

	if (isDds)
	{
		CheckHR(DirectX::CreateDDSTextureFromMemoryEx(*device,
													  device.GetResourceUpload(),
													  static_cast<const std::byte*>(data),
													  size,
													  0,
													  D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
													  generateMips ? DirectX::DDS_LOADER_MIP_AUTOGEN
																   : DirectX::DDS_LOADER_IGNORE_MIPS,
													  &m_resource,
													  nullptr,
													  &isCubemap));
	}
	else
	{
		CheckHR(DirectX::CreateWICTextureFromMemoryEx(*device,
													  device.GetResourceUpload(),
													  static_cast<const std::byte*>(data),
													  size,
													  0,
													  D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
													  generateMips ? DirectX::WIC_LOADER_MIP_AUTOGEN
																   : DirectX::WIC_LOADER_DEFAULT,
													  &m_resource));
	}

	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	SetState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	const D3D12_RESOURCE_DESC desc = m_resource->GetDesc();
	m_textureType				   = isCubemap ? TextureType::DCube : DxToTextureType(desc.Dimension, desc.DepthOrArraySize);

	if (!_internal)
		GenerateDescriptors(device, true, isCubemap);
}

DxTexture::~DxTexture()
{
	if (m_device)
	{
		m_device->GetShaderDescriptorPile().Free(m_srvHeapIndex, 1);
		for (const auto uavIdx : m_uavHeapIndices)
			m_device->GetShaderDescriptorPile().Free(uavIdx, 1);
	}
}

ID3D12Resource* DxTexture::GetResource() const { return m_resource.Get(); }
ID3D12Resource* DxTexture::operator*() const { return m_resource.Get(); }
ID3D12Resource* DxTexture::operator->() const { return m_resource.Get(); }

size_t DxTexture::GetWidth() const { return m_resource->GetDesc().Width; }
size_t DxTexture::GetHeight() const { return m_resource->GetDesc().Height; }
size_t DxTexture::GetDepthOrArraySize() const { return m_resource->GetDesc().DepthOrArraySize; }
uint8_t	 DxTexture::GetNumChannels() const { return GetChannelCount(m_resource->GetDesc().Format); }
uint32_t DxTexture::GetNumMips() const { return m_resource->GetDesc().MipLevels; }

void DxTexture::CreateUAV(const DxDevice& device, uint32_t mip, uint32_t sliceMin, uint32_t sliceMax)
{
	// If no UAV was generated for mip 0, the proper flags are not set. This is not supported
	if (m_uavHeapIndices.empty())
		Log::Critical("UAV flags not set for resource. Undefined behaviour.");

	auto&	   descriptorPile = device.GetShaderDescriptorPile();
	const auto uavIdx		  = descriptorPile.Allocate();
	m_uavHeapIndices.push_back(uavIdx);

	const auto uav = descriptorPile.GetCpuHandle(uavIdx);

	CD3DX12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};

	switch (m_textureType)
	{
	case TextureType::Invalid:
		return;
	case TextureType::D1:
		uavDesc = CD3DX12_UNORDERED_ACCESS_VIEW_DESC::Tex1D(GetFormat(), mip);
		break;
	case TextureType::D1Array:
		uavDesc = CD3DX12_UNORDERED_ACCESS_VIEW_DESC::Tex1DArray(GetFormat(), sliceMax - sliceMin + 1, sliceMax, mip);
		break;
	case TextureType::D2:
		uavDesc = CD3DX12_UNORDERED_ACCESS_VIEW_DESC::Tex2D(GetFormat(), mip);
		break;
	case TextureType::D2Array:
		uavDesc = CD3DX12_UNORDERED_ACCESS_VIEW_DESC::Tex2DArray(GetFormat(), sliceMax - sliceMin + 1, sliceMax, mip);
		break;
	case TextureType::D3:
		uavDesc = CD3DX12_UNORDERED_ACCESS_VIEW_DESC::Tex3D(GetFormat(), -1, 0, mip);
		break;
	case TextureType::DCube:
		uavDesc = CD3DX12_UNORDERED_ACCESS_VIEW_DESC::Tex2DArray(GetFormat(), 6, 0, mip);
		break;
	}

	device->CreateUnorderedAccessView(GetResource(), nullptr, &uavDesc, uav);
}

int32_t DxTexture::GetSrvHeapIndex() const { return m_srvHeapIndex; }
int32_t DxTexture::GetUavHeapIndex(const uint32_t idx) const { return m_uavHeapIndices.at(idx); }
bool	DxTexture::IsValid() const { return m_resource.Get() != nullptr && m_textureType != TextureType::Invalid; }

void DxTexture::GenerateDescriptors(const DxDevice& device, const bool generateSrv, const bool isCubemap)
{
	// Generate descriptors
	auto& descriptorPile = device.GetShaderDescriptorPile();

	if (generateSrv)
	{
		m_srvHeapIndex = descriptorPile.Allocate();

		const auto srv = descriptorPile.GetCpuHandle(m_srvHeapIndex);
		DirectX::CreateShaderResourceView(*device, m_resource.Get(), srv, isCubemap);
	}
}
