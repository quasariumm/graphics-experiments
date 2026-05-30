module;

module dx_wrapper.resources.dx_texture;
import std;
import dx_wrapper.rendering.dx_descriptor_pile;
import dx_wrapper.core;
import dx_wrapper.external.directxtex;
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

DxTexture::DxTexture(const DxDevice& device, const std::filesystem::path& path, const bool generateMips,
					 D3D12_RESOURCE_FLAGS additionalFlags)
{
	m_device = &device;

	bool isCubemap = false;

	DirectX::ScratchImage image;
	DirectX::TexMetadata  metadata;

	const auto extension = path.extension();

	if (extension == ".hdr")
		CheckHR(DirectX::LoadFromHDRFile(path.c_str(), &metadata, image));
	else if (extension == ".exr")
		CheckHR(DirectX::LoadFromEXRFile(path.c_str(), &metadata, image));
	else if (extension == ".dds" || extension == ".dxg")
		CheckHR(DirectX::LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, &metadata, image));
	else
		CheckHR(DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image));
	isCubemap = metadata.IsCubemap();

	if (generateMips && image.GetMetadata().mipLevels == 1)
	{
		DirectX::ScratchImage mipChain;
		CheckHR(DirectX::GenerateMipMaps(*image.GetImage(0, 0, 0), DirectX::TEX_FILTER_DEFAULT, 0, mipChain));
		image	 = std::move(mipChain);
		metadata = image.GetMetadata();
	}

	auto flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (additionalFlags != D3D12_RESOURCE_FLAG_NONE)
		flags = static_cast<D3D12_RESOURCE_FLAGS>(flags | additionalFlags);

	CheckHR(DirectX::CreateTextureEx(*device, metadata, flags, DirectX::CREATETEX_DEFAULT, m_resource.GetAddressOf()));

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	CheckHR(DirectX::PrepareUpload(*device, image.GetImages(), image.GetImageCount(), metadata, subresources));

	device.GetResourceUpload().Begin();
	device.GetResourceUpload().Upload(m_resource.Get(), 0, subresources.data(), subresources.size());
	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	SetState(D3D12_RESOURCE_STATE_COPY_DEST);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

#ifdef _MSC_VER
	const D3D12_RESOURCE_DESC desc = m_resource->GetDesc();
#else
	D3D12_RESOURCE_DESC desc;
	m_resource->GetDesc(&desc);
#endif
	m_textureType = isCubemap ? TextureType::DCube : DxToTextureType(desc.Dimension, desc.DepthOrArraySize);

	UpdateOrCreateSrv(device, isCubemap);
}

DxTexture::DxTexture(const DxDevice& device, const std::byte* data, const TextureType type, const DXGI_FORMAT format,
					 const std::uint32_t width, const std::uint32_t height, const std::uint32_t depth, bool generateMips,
					 D3D12_RESOURCE_FLAGS additionalFlags)
{
	m_textureType = type;

	const bool is3D	  = TextureTypeToDx(type) == D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	const bool isCube = type == TextureType::DCube;

	DirectX::TexMetadata metadata{};
	metadata.width	   = width;
	metadata.height	   = height;
	metadata.depth	   = is3D ? depth : 1;
	metadata.arraySize = is3D ? 1 : depth;
	metadata.mipLevels = 1;
	metadata.format	   = format;
	metadata.dimension = TextureTypeToDxTex(type);
	metadata.miscFlags = isCube ? DirectX::TEX_MISC_TEXTURECUBE : 0;

	DirectX::ScratchImage image;
	CheckHR(image.Initialize(metadata));
	memcpy(image.GetPixels(), data, image.GetPixelsSize());

	if (generateMips)
	{
		DirectX::ScratchImage mipChain;
		if (is3D)
			CheckHR(DirectX::GenerateMipMaps3D(image.GetImages(),
											   image.GetImageCount(),
											   metadata,
											   DirectX::TEX_FILTER_DEFAULT,
											   0,
											   mipChain));
		else
			CheckHR(DirectX::GenerateMipMaps(image.GetImages(),
											 image.GetImageCount(),
											 metadata,
											 DirectX::TEX_FILTER_DEFAULT,
											 0,
											 mipChain));
		image	 = std::move(mipChain);
		metadata = image.GetMetadata();
	}

	auto flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (additionalFlags != D3D12_RESOURCE_FLAG_NONE)
		flags = static_cast<D3D12_RESOURCE_FLAGS>(flags | additionalFlags);

	CheckHR(DirectX::CreateTextureEx(*device, metadata, flags, DirectX::CREATETEX_DEFAULT, m_resource.GetAddressOf()));

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	CheckHR(DirectX::PrepareUpload(*device, image.GetImages(), image.GetImageCount(), metadata, subresources));

	device.GetResourceUpload().Begin();
	device.GetResourceUpload().Upload(m_resource.Get(),
									  0,
									  subresources.data(),
									  static_cast<std::uint32_t>(subresources.size()));
	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	SetState(D3D12_RESOURCE_STATE_COPY_DEST);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	UpdateOrCreateSrv(device, isCube);
}

DxTexture::DxTexture(const DxDevice& device, TextureType type, DXGI_FORMAT format, std::uint32_t width, std::uint32_t height,
					 std::uint32_t depth, bool allocateMips, D3D12_RESOURCE_FLAGS additionalFlags)
{
	m_textureType = type;

	std::size_t mips = 1;
	DirectX::CalculateMipLevels(width, height, mips);

	auto flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (additionalFlags != D3D12_RESOURCE_FLAG_NONE)
		flags = static_cast<D3D12_RESOURCE_FLAGS>(flags | additionalFlags);

	D3D12_RESOURCE_DESC texDesc{};
	texDesc.Dimension		 = TextureTypeToDx(type);
	texDesc.Width			 = width;
	texDesc.Height			 = height;
	texDesc.DepthOrArraySize = depth;
	texDesc.Format			 = format;
	texDesc.Flags			 = flags;
	texDesc.Layout			 = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.MipLevels		 = allocateMips ? mips : 1;
	texDesc.Alignment		 = 0;

	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;

	const CD3DX12_HEAP_PROPERTIES heapProp{D3D12_HEAP_TYPE_DEFAULT};

	CheckHR(device->CreateCommittedResource(&heapProp,
											D3D12_HEAP_FLAG_NONE,
											&texDesc,
											D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
											nullptr,
											GetIID(m_resource),
											GetPPV(m_resource)));

	UpdateOrCreateSrv(device, type == TextureType::DCube);
}

DxTexture::DxTexture(const DxDevice& device, const std::byte* data, const std::size_t size, const bool generateMips,
					 D3D12_RESOURCE_FLAGS additionalFlags)
{
	const bool isHdr = size >= 2 && *reinterpret_cast<const std::uint16_t*>(data) == 0x3f23;
	const bool isExr = size >= 4 && *reinterpret_cast<const std::uint32_t*>(data) == 0x01312f76;
	const bool isDds = size >= 4 && *reinterpret_cast<const std::uint32_t*>(data) == 0x20534444;

	bool				  isCubemap = false;
	DirectX::ScratchImage image;
	DirectX::TexMetadata  metadata;

	if (isHdr)
		CheckHR(DirectX::LoadFromHDRMemory(data, size, &metadata, image));
	else if (isExr)
		CheckHR(DirectX::LoadFromEXRMemory(reinterpret_cast<const std::uint8_t*>(data), size, &metadata, image));
	else if (isDds)
		CheckHR(DirectX::LoadFromDDSMemory(data, size, DirectX::DDS_FLAGS_NONE, &metadata, image));
	else
		CheckHR(DirectX::LoadFromWICMemory(data, size, DirectX::WIC_FLAGS_NONE, &metadata, image));
	isCubemap = metadata.IsCubemap();

	if (generateMips && image.GetMetadata().mipLevels == 1)
	{
		DirectX::ScratchImage mipChain;
		CheckHR(DirectX::GenerateMipMaps(*image.GetImage(0, 0, 0), DirectX::TEX_FILTER_DEFAULT, 0, mipChain));
		image	 = std::move(mipChain);
		metadata = image.GetMetadata();
	}

	auto flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (additionalFlags != D3D12_RESOURCE_FLAG_NONE)
		flags = static_cast<D3D12_RESOURCE_FLAGS>(flags | additionalFlags);

	CheckHR(DirectX::CreateTextureEx(*device, metadata, flags, DirectX::CREATETEX_DEFAULT, &m_resource));

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	CheckHR(DirectX::PrepareUpload(*device, image.GetImages(), image.GetImageCount(), metadata, subresources));

	device.GetResourceUpload().Begin();
	device.GetResourceUpload().Upload(m_resource.Get(), 0, subresources.data(), subresources.size());
	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	SetState(D3D12_RESOURCE_STATE_COPY_DEST);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

#ifdef _MSC_VER
	const D3D12_RESOURCE_DESC desc = m_resource->GetDesc();
#else
	D3D12_RESOURCE_DESC desc;
	m_resource->GetDesc(&desc);
#endif
	m_textureType = isCubemap ? TextureType::DCube : DxToTextureType(desc.Dimension, desc.DepthOrArraySize);

	UpdateOrCreateSrv(device, isCubemap);
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

void DxTexture::Resize(const DxDevice& device, std::uint32_t width, std::uint32_t height, std::uint32_t depth)
{
	auto oldResource = m_resource;

	// Make the new resource
	ResizeClear(device, width, height, depth);

	// Copy over (a region of) the old resource into the new
	const CD3DX12_TEXTURE_COPY_LOCATION dst{m_resource.Get()};
	const CD3DX12_TEXTURE_COPY_LOCATION src{oldResource.Get()};

	const auto		  desc = GetDesc(oldResource.Get());
	const CD3DX12_BOX region{
			0,
			0,
			0,
			static_cast<long>(std::min(desc.Width, static_cast<UINT64>(width))),
			static_cast<long>(std::min(desc.Height, height)),
			std::min(desc.DepthOrArraySize, static_cast<UINT16>(depth)),
	};

	device.GetDXDirectComList()->CopyTextureRegion(&dst, 0, 0, 0, &src, &region);
}

void DxTexture::ResizeClear(const DxDevice& device, std::uint32_t width, std::uint32_t height, std::uint32_t depth)
{
	auto desc			  = GetDesc(m_resource.Get());
	desc.Width			  = width;
	desc.Height			  = height;
	desc.DepthOrArraySize = depth;

	const CD3DX12_HEAP_PROPERTIES heapProp{D3D12_HEAP_TYPE_DEFAULT};

	m_resource = nullptr;

	SetState(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	CheckHR(device->CreateCommittedResource(&heapProp,
											D3D12_HEAP_FLAG_NONE,
											&desc,
											D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
											nullptr,
											GetIID(m_resource.GetAddressOf()),
											GetPPV(m_resource.GetAddressOf())));

	// Point all descriptors to the correct location
	UpdateOrCreateSrv(device, m_textureType == TextureType::DCube, true);
	for (int i = 0; i < m_uavDescs.size(); ++i)
		UpdateOrCreateUav(device, 0, 0, 0, true, i);
	
	DerivedUpdateDescriptors(device);
}

std::size_t DxTexture::GetWidth() const
{
#ifdef _MSC_VER
	return m_resource->GetDesc().Width;
#else
	D3D12_RESOURCE_DESC desc;
	m_resource->GetDesc(&desc);
	return desc.Width;
#endif
}

std::size_t DxTexture::GetHeight() const
{
#ifdef _MSC_VER
	return m_resource->GetDesc().Height;
#else
	D3D12_RESOURCE_DESC desc;
	m_resource->GetDesc(&desc);
	return desc.Height;
#endif
}

std::size_t DxTexture::GetDepthOrArraySize() const
{
#ifdef _MSC_VER
	return m_resource->GetDesc().DepthOrArraySize;
#else
	D3D12_RESOURCE_DESC desc;
	m_resource->GetDesc(&desc);
	return desc.DepthOrArraySize;
#endif
}

std::uint8_t DxTexture::GetNumChannels() const
{
#ifdef _MSC_VER
	return GetChannelCount(m_resource->GetDesc().Format);
#else
	D3D12_RESOURCE_DESC desc;
	m_resource->GetDesc(&desc);
	return GetChannelCount(desc.Format);
#endif
}

std::uint32_t DxTexture::GetNumMips() const
{
#ifdef _MSC_VER
	return m_resource->GetDesc().MipLevels;
#else
	D3D12_RESOURCE_DESC desc;
	m_resource->GetDesc(&desc);
	return desc.MipLevels;
#endif
}

void DxTexture::CreateUAV(const DxDevice& device, std::uint32_t mip, std::uint32_t sliceMin, std::uint32_t sliceMax)
{ UpdateOrCreateUav(device, mip, sliceMin, sliceMax); }

std::int32_t DxTexture::GetSrvHeapIndex() const { return m_srvHeapIndex; }
std::int32_t DxTexture::GetUavHeapIndex(const std::uint32_t idx) const { return m_uavHeapIndices.at(idx); }
bool		 DxTexture::IsValid() const { return m_resource.Get() != nullptr && m_textureType != TextureType::Invalid; }

void DxTexture::UpdateOrCreateSrv(const DxDevice& device, bool isCubemap, bool update)
{
	// Generate descriptors
	auto& descriptorPile = device.GetShaderDescriptorPile();

	// Allocate index when creating the SRV
	if (!update)
		m_srvHeapIndex = descriptorPile.Allocate();

	const auto srv = descriptorPile.GetCpuHandleAt(m_srvHeapIndex);

	switch (m_textureType)
	{
	case TextureType::Invalid:
		return;
	case TextureType::D1:
		m_srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex1D(GetFormat());
		break;
	case TextureType::D1Array:
		m_srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex1DArray(GetFormat());
		break;
	case TextureType::D2:
		m_srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex2D(GetFormat());
		break;
	case TextureType::D2Array:
		m_srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex2DArray(GetFormat());
		break;
	case TextureType::D3:
		m_srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex3D(GetFormat());
		break;
	case TextureType::DCube:
		m_srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex2DArray(GetFormat(), 6);
		break;
	}

	device->CreateShaderResourceView(m_resource.Get(), &m_srvDesc, srv);
}

void DxTexture::UpdateOrCreateUav(const DxDevice& device, std::uint32_t mip, std::uint32_t sliceMin, std::uint32_t sliceMax,
								  bool update, std::size_t updateIndex)
{
	auto& descriptorPile = device.GetShaderDescriptorPile();

	const auto uavIdx = (update) ? m_uavHeapIndices.at(updateIndex) : descriptorPile.Allocate();

	if (!update)
		m_uavHeapIndices.push_back(uavIdx);

	const auto uav = descriptorPile.GetCpuHandleAt(uavIdx);

	CD3DX12_UNORDERED_ACCESS_VIEW_DESC& uavDesc = (update) ? m_uavDescs.at(updateIndex) : m_uavDescs.emplace_back();

	if (update)
	{
		// Write to the mip and slice params based on the desc
		switch (uavDesc.ViewDimension)
		{
		case D3D12_UAV_DIMENSION_TEXTURE1D:
			mip		 = uavDesc.Texture1D.MipSlice;
			sliceMin = 0;
			sliceMax = 1;
			break;
		case D3D12_UAV_DIMENSION_TEXTURE2D:
			mip		 = uavDesc.Texture2D.MipSlice;
			sliceMin = 0;
			sliceMax = 1;
			break;
		case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
			mip		 = uavDesc.Texture2DArray.MipSlice;
			sliceMin = uavDesc.Texture2DArray.FirstArraySlice;
			sliceMax = uavDesc.Texture2DArray.FirstArraySlice + uavDesc.Texture2DArray.ArraySize;
			break;
		case D3D12_UAV_DIMENSION_TEXTURE3D:
			mip		 = uavDesc.Texture3D.MipSlice;
			sliceMin = uavDesc.Texture3D.FirstWSlice;
			sliceMax = uavDesc.Texture3D.FirstWSlice + uavDesc.Texture3D.WSize;
			break;
		default:
			break;
		}
	}

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
