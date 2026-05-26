module;

export module dx_wrapper.helpers.dx_buffer_helpers;
import std;
import dx_wrapper.core;
import dx_wrapper.rendering.dx_descriptor_heap;
import dx_wrapper.rendering.dx_descriptor_pile;

// NOLINTNEXTLINE
static constexpr auto D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE = static_cast<D3D12_RESOURCE_STATES>(4194304);

export HRESULT CreateStaticBuffer(DxDevice& device, const void* data, const std::size_t size,
								  const D3D12_RESOURCE_STATES initialState, ComPtr<ID3D12Resource>& buffer,
								  const D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE)
{
	// Create
	const auto heapProps	= CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size, resourceFlags);

	D3D12_RESOURCE_STATES creationState = D3D12_RESOURCE_STATE_COMMON;
	if (initialState == D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE)
		creationState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;

	const HRESULT result = device->CreateCommittedResource(&heapProps,
														   D3D12_HEAP_FLAG_NONE,
														   &resourceDesc,
														   creationState,
														   nullptr,
														   GetIID(buffer),
														   GetPPV(buffer));

	if (FAILED(result))
		return result;

	// Transition
	if (initialState != creationState)
	{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COMMON, initialState);
		device.GetDXDirectComList()->ResourceBarrier(1, &barrier);
	}

	if (!data)
		return result;

	// Skip ray acceleration structures since that can never be COPY_DEST
	if (initialState == D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE)
		return result;

	// Upload
	D3D12_SUBRESOURCE_DATA initData;
	initData.pData		= data;
	initData.RowPitch	= size;
	initData.SlicePitch = initData.RowPitch;
	device.GetResourceUpload().Begin();
	device.GetResourceUpload().Upload(buffer.Get(), 0, &initData, 1);
	device.GetResourceUpload().End(device.GetDXDirectComQueue());

	return result;
}

export template <typename T>
HRESULT CreateStaticBuffer(DxDevice& device, const std::vector<T>& data, const D3D12_RESOURCE_STATES initialState,
						   ComPtr<ID3D12Resource>& buffer, const D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE)
{ return CreateStaticBuffer(device, data.data(), sizeof(T) * data.size(), initialState, buffer, resourceFlags); }

/*
 * Private descriptor helpers
 */

void CreateSRV(const DxDevice& device, ID3D12Resource* resource, const D3D12_CPU_DESCRIPTOR_HANDLE handle,
			   size_t structuredBufferElementSize)
{
#ifdef _MSC_VER
	const auto desc = resource->GetDesc();
#else
	D3D12_RESOURCE_DESC desc;
	resource->GetDesc(&desc);
#endif

	switch (desc.Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_UNKNOWN:
		Log::Error("SRV cannot be created for an unknown/uninitialized resource.");
		break;
	case D3D12_RESOURCE_DIMENSION_BUFFER:
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.ViewDimension			= D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		if (structuredBufferElementSize == 0)
		{
			// User wants a ByteAddressBuffer / Buffer<uint>
			srvDesc.Format					   = DXGI_FORMAT_R32_TYPELESS;
			srvDesc.Buffer.FirstElement		   = 0;
			srvDesc.Buffer.NumElements		   = desc.Width / 4;
			srvDesc.Buffer.StructureByteStride = 0;
			srvDesc.Buffer.Flags			   = D3D12_BUFFER_SRV_FLAG_RAW;
		}
		else
		{
			// User wants a StructuredBuffer<T>
			srvDesc.Format					   = DXGI_FORMAT_UNKNOWN;
			srvDesc.Buffer.FirstElement		   = 0;
			srvDesc.Buffer.NumElements		   = desc.Width / structuredBufferElementSize;
			srvDesc.Buffer.StructureByteStride = structuredBufferElementSize;
			srvDesc.Buffer.Flags			   = D3D12_BUFFER_SRV_FLAG_NONE;
		}
		device->CreateShaderResourceView(resource, &srvDesc, handle);
		break;
	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		device->CreateShaderResourceView(resource, nullptr, handle);
		break;
	}
}

void CreateUAV(const DxDevice& device, ID3D12Resource* resource, const D3D12_CPU_DESCRIPTOR_HANDLE handle,
			   size_t structuredBufferElementSize)
{
#ifdef _MSC_VER
	const auto desc = resource->GetDesc();
#else
	D3D12_RESOURCE_DESC desc;
	resource->GetDesc(&desc);
#endif

	switch (desc.Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_UNKNOWN:
		Log::Error("UAV cannot be created for an unknown/uninitialized resource.");
		break;
	case D3D12_RESOURCE_DIMENSION_BUFFER:
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		if (structuredBufferElementSize == 0)
		{
			// User wants a ByteAddressBuffer / Buffer<uint>
			uavDesc.Format					   = DXGI_FORMAT_R32_TYPELESS;
			uavDesc.Buffer.NumElements		   = desc.Width / 4;
			uavDesc.Buffer.StructureByteStride = 0;
			uavDesc.Buffer.Flags			   = D3D12_BUFFER_UAV_FLAG_RAW;
		}
		else
		{
			// User wants a StructuredBuffer<T>
			uavDesc.Format					   = DXGI_FORMAT_UNKNOWN;
			uavDesc.Buffer.FirstElement		   = 0;
			uavDesc.Buffer.NumElements		   = desc.Width / structuredBufferElementSize;
			uavDesc.Buffer.StructureByteStride = structuredBufferElementSize;
			uavDesc.Buffer.Flags			   = D3D12_BUFFER_UAV_FLAG_NONE;
		}
		device->CreateUnorderedAccessView(resource, nullptr, &uavDesc, handle);
		break;
	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		device->CreateUnorderedAccessView(resource, nullptr, nullptr, handle);
		break;
	}
}

/*
 * Descriptor creation functions
 */

/**
 * @brief Allocates a new SRV on the shader descriptor pile in the device
 * @param device The device
 * @param resource The resource
 * @param structuredBufferElementSize [optional, applies only to buffer resources] If you want the resource you pass in to be a
 * StructuredBuffer, specify the element stride here. Leave 0 to create a ByteAddressBuffer
 * @return The index in the pile
 */
export DxDescriptorPile::IndexType AllocateShaderResourceView(DxDevice& device, ID3D12Resource* resource,
															  std::size_t structuredBufferElementSize = 0)
{
	const auto heapIndex = device.GetShaderDescriptorPile().Allocate();
	const auto result	 = device.GetShaderDescriptorPile().GetCpuHandleAt(heapIndex);
	CreateSRV(device, resource, result, structuredBufferElementSize);
	return heapIndex;
}

/**
 * @brief Allocates a new UAV on the shader descriptor pile in the device
 * @param device The device
 * @param resource The resource
 * @param structuredBufferElementSize [optional, applies only to buffer resources] If you want the resource you pass in to be a
 * StructuredBuffer, specify the element stride here. Leave 0 to create a ByteAddressBuffer
 * @return The index in the pile
 */
export DxDescriptorPile::IndexType AllocateUnorderedAccessView(DxDevice& device, ID3D12Resource* resource,
															   std::size_t structuredBufferElementSize = 0)
{
	const auto heapIndex = device.GetShaderDescriptorPile().Allocate();
	const auto result	 = device.GetShaderDescriptorPile().GetCpuHandleAt(heapIndex);
	CreateUAV(device, resource, result, structuredBufferElementSize);
	return heapIndex;
}

/**
 * @brief Allocates a new CBV on the shader descriptor pile in the device
 * @param device The device
 * @param resource The resource
 * @return The index in the pile
 */
export DxDescriptorPile::IndexType AllocateConstantBufferView(DxDevice& device, ID3D12Resource* resource)
{
	const auto						heapIndex = device.GetShaderDescriptorPile().Allocate();
	const auto						result	  = device.GetShaderDescriptorPile().GetCpuHandleAt(heapIndex);
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
	desc.BufferLocation = resource->GetGPUVirtualAddress();
#ifdef _MSC_VER
	desc.SizeInBytes = resource->GetDesc().Width;
#else
	D3D12_RESOURCE_DESC resourceDesc;
	resource->GetDesc(&resourceDesc);
	desc.SizeInBytes = resourceDesc.Width;
#endif
	device->CreateConstantBufferView(&desc, result);
	return heapIndex;
}

/**
 * @brief Creates a new SRV in the given handle
 * @param device The device
 * @param resource The resource
 * @param structuredBufferElementSize [optional, applies only to buffer resources] If you want the resource you pass in to be a
 * StructuredBuffer, specify the element stride here. Leave 0 to create a ByteAddressBuffer
 * @return The index in the pile
 */
export void CreateShaderResourceView(const DxDevice& device, const DxDescriptorHeap& heap, const size_t index,
									 ID3D12Resource* resource, std::size_t structuredBufferElementSize = 0)
{
	const auto result = heap.GetCpuHandleAt(index);
	CreateSRV(device, resource, result, structuredBufferElementSize);
}

/**
 * @brief Creates a new UAV in the given handle
 * @param device The device
 * @param resource The resource
 * @param structuredBufferElementSize [optional, applies only to buffer resources] If you want the resource you pass in to be a
 * StructuredBuffer, specify the element stride here. Leave 0 to create a ByteAddressBuffer
 * @return The index in the pile
 */
export void CreateUnorderedAccessView(const DxDevice& device, const DxDescriptorHeap& heap, const size_t index,
									  ID3D12Resource* resource, std::size_t structuredBufferElementSize = 0)
{
	const auto result = heap.GetCpuHandleAt(index);
	CreateUAV(device, resource, result, structuredBufferElementSize);
}

/**
 * @brief Creates a new CBV in the given handle
 * @param device The device
 * @param resource The resource
 * @return The index in the pile
 */
export void CreateConstantBufferView(const DxDevice& device, const DxDescriptorHeap& heap, const size_t index,
									 ID3D12Resource* resource)
{
	const auto						result = heap.GetCpuHandleAt(index);
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
	desc.BufferLocation = resource->GetGPUVirtualAddress();
#ifdef _MSC_VER
	desc.SizeInBytes = resource->GetDesc().Width;
#else
	D3D12_RESOURCE_DESC resourceDesc;
	resource->GetDesc(&resourceDesc);
	desc.SizeInBytes = resourceDesc.Width;
#endif
	device->CreateConstantBufferView(&desc, result);
}
