module;

#include <d3d12.h>
#include <d3dx12.h>
#include <filesystem>
#include <wincodec.h>

#include <DirectXTex.h>

module dx_wrapper.resources.dx_texture;
import dx_wrapper.rendering.dx_descriptor_pile;
import dx_wrapper.core;
import dx_wrapper.helpers.dx_format_helpers;

inline TextureType DxToTextureType(const D3D12_RESOURCE_DIMENSION dim, const UINT depthOrArraySize)
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

inline D3D12_RESOURCE_DIMENSION TextureTypeToDx(const TextureType type)
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

inline DirectX::TEX_DIMENSION TextureTypeToDxTex(const TextureType type)
{
	switch (type)
	{
	case TextureType::Invalid:
		return DirectX::TEX_DIMENSION_TEXTURE2D;
	case TextureType::D1:
	case TextureType::D1Array:
		return DirectX::TEX_DIMENSION_TEXTURE1D;
	case TextureType::D2:
	case TextureType::D2Array:
	case TextureType::DCube:
		return DirectX::TEX_DIMENSION_TEXTURE2D;
	case TextureType::D3:
		return DirectX::TEX_DIMENSION_TEXTURE3D;
	}
	return DirectX::TEX_DIMENSION_TEXTURE2D;
}

DxTexture::DxTexture(const DxDevice& device, const std::filesystem::path& path, const bool generateMips, const bool _internal)
{
	m_device = &device;

	bool isCubemap = false;

	DirectX::ScratchImage image;
	DirectX::TexMetadata  metadata;

	if (path.extension() == ".dds" || path.extension() == ".dxg")
	{
		CheckHR(DirectX::LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, &metadata, image));
		isCubemap = metadata.IsCubemap();
	}
	else
	{
		CheckHR(DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image));
	}

	if (generateMips && image.GetMetadata().mipLevels == 1)
	{
		DirectX::ScratchImage mipChain;
		CheckHR(DirectX::GenerateMipMaps(*image.GetImage(0, 0, 0), DirectX::TEX_FILTER_DEFAULT, 0, mipChain));
		image	 = std::move(mipChain);
		metadata = image.GetMetadata();
	}

	CheckHR(DirectX::CreateTextureEx(*device,
									 metadata,
									 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
									 DirectX::CREATETEX_DEFAULT,
									 m_resource.GetAddressOf()));

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	CheckHR(DirectX::PrepareUpload(*device, image.GetImages(), image.GetImageCount(), metadata, subresources));
	
	device.GetResourceUpload().Begin();
	device.GetResourceUpload().Upload(m_resource.Get(), 0, subresources.data(), subresources.size());
	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	SetState(D3D12_RESOURCE_STATE_COPY_DEST);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	const D3D12_RESOURCE_DESC desc = m_resource->GetDesc();
	m_textureType				   = isCubemap ? TextureType::DCube : DxToTextureType(desc.Dimension, desc.DepthOrArraySize);

	if (!_internal)
		GenerateDescriptors(device, true, isCubemap);
}

DxTexture::DxTexture(const DxDevice& device, const std::byte* data, const TextureType type, const DXGI_FORMAT format,
					 const uint32_t width, const uint32_t height, const uint32_t depth, bool generateMips, const bool _internal)
{
	m_textureType = type;

	const bool is3D  = TextureTypeToDx(type) == D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	const bool isCube = type == TextureType::DCube;

	DirectX::TexMetadata metadata{};
	metadata.width      = width;
	metadata.height     = height;
	metadata.depth      = is3D ? depth : 1;
	metadata.arraySize  = is3D ? 1 : depth;
	metadata.mipLevels  = 1;
	metadata.format     = format;
	metadata.dimension  = TextureTypeToDxTex(type);
	metadata.miscFlags  = isCube ? DirectX::TEX_MISC_TEXTURECUBE : 0;

	DirectX::ScratchImage image;
	CheckHR(image.Initialize(metadata));
	memcpy(image.GetPixels(), data, image.GetPixelsSize());

	if (generateMips)
	{
		DirectX::ScratchImage mipChain;
		if (is3D)
			CheckHR(DirectX::GenerateMipMaps3D(image.GetImages(), image.GetImageCount(), metadata, DirectX::TEX_FILTER_DEFAULT, 0, mipChain));
		else
			CheckHR(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), metadata, DirectX::TEX_FILTER_DEFAULT, 0, mipChain));
		image    = std::move(mipChain);
		metadata = image.GetMetadata();
	}

	CheckHR(DirectX::CreateTextureEx(
		*device,
		metadata,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		DirectX::CREATETEX_DEFAULT,
		m_resource.GetAddressOf()));

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	CheckHR(DirectX::PrepareUpload(*device, image.GetImages(), image.GetImageCount(), metadata, subresources));

	device.GetResourceUpload().Begin();
	device.GetResourceUpload().Upload(m_resource.Get(), 0, subresources.data(), static_cast<uint32_t>(subresources.size()));
	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	SetState(D3D12_RESOURCE_STATE_COPY_DEST);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	if (!_internal)
		GenerateDescriptors(device, true, isCube);
}

DxTexture::DxTexture(const DxDevice& device, TextureType type, DXGI_FORMAT format, uint32_t width, uint32_t height,
					 uint32_t depth, bool allocateMips, const bool _internal)
{
	size_t mips = 1;
	DirectX::CalculateMipLevels(width, height, mips);
	
	D3D12_RESOURCE_DESC texDesc{};
	texDesc.Dimension		 = TextureTypeToDx(type);
	texDesc.Width			 = width;
	texDesc.Height			 = height;
	texDesc.DepthOrArraySize = depth;
	texDesc.Format			 = format;
	texDesc.Flags			 = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	texDesc.Layout			 = D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE;
	texDesc.MipLevels		 = allocateMips ? mips : 1;
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

DxTexture::DxTexture(const DxDevice& device, const std::byte* data, const size_t size, const bool generateMips,
					 const bool _internal)
{
	const bool isDds = size >= 4 && *reinterpret_cast<const uint32_t*>(data) == 0x20534444;

	bool				  isCubemap = false;
	DirectX::ScratchImage image;
	DirectX::TexMetadata  metadata;

	if (isDds)
	{
		CheckHR(DirectX::LoadFromDDSMemory(data, size, DirectX::DDS_FLAGS_NONE, &metadata, image));
		isCubemap = metadata.IsCubemap();
	}
	else
	{
		CheckHR(DirectX::LoadFromWICMemory(data, size, DirectX::WIC_FLAGS_NONE, &metadata, image));
	}

	if (generateMips && image.GetMetadata().mipLevels == 1)
	{
		DirectX::ScratchImage mipChain;
		CheckHR(DirectX::GenerateMipMaps(*image.GetImage(0, 0, 0), DirectX::TEX_FILTER_DEFAULT, 0, mipChain));
		image	 = std::move(mipChain);
		metadata = image.GetMetadata();
	}

	CheckHR(DirectX::CreateTextureEx(*device,
									 metadata,
									 D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
									 DirectX::CREATETEX_DEFAULT,
									 &m_resource));

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	CheckHR(DirectX::PrepareUpload(*device, image.GetImages(), image.GetImageCount(), metadata, subresources));
	
	device.GetResourceUpload().Begin();
	device.GetResourceUpload().Upload(m_resource.Get(), 0, subresources.data(), subresources.size());
	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	SetState(D3D12_RESOURCE_STATE_COPY_DEST);
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

size_t	 DxTexture::GetWidth() const { return m_resource->GetDesc().Width; }
size_t	 DxTexture::GetHeight() const { return m_resource->GetDesc().Height; }
size_t	 DxTexture::GetDepthOrArraySize() const { return m_resource->GetDesc().DepthOrArraySize; }
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

	const auto uav = descriptorPile.GetCpuHandleAt(uavIdx);

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

		const auto srv = descriptorPile.GetCpuHandleAt(m_srvHeapIndex);

		CD3DX12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

		switch (m_textureType)
		{
		case TextureType::Invalid:
			return;
		case TextureType::D1:
			srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex1D(GetFormat());
			break;
		case TextureType::D1Array:
			srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex1DArray(GetFormat());
			break;
		case TextureType::D2:
			srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex2D(GetFormat());
			break;
		case TextureType::D2Array:
			srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex2DArray(GetFormat());
			break;
		case TextureType::D3:
			srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex3D(GetFormat());
			break;
		case TextureType::DCube:
			srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex2DArray(GetFormat(), 6);
			break;
		}

		device->CreateShaderResourceView(m_resource.Get(), &srvDesc, srv);
	}
}
