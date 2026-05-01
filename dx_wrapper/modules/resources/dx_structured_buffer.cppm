module;

#include <vector>
#include "macros.hpp"

export module dx_wrapper.resources.dx_structured_buffer;
import dx_wrapper.resources.dx_resource;
import dx_wrapper.core;

export template <typename T>
class DxStructuredBuffer : protected DxResource
{

public:

	DxStructuredBuffer() = default;

	explicit DxStructuredBuffer(DxDevice& device, uint32_t numElements);
	explicit DxStructuredBuffer(DxDevice& device, uint32_t numElements, T value);
	explicit DxStructuredBuffer(DxDevice& device, const std::vector<T>& data);
	
	ID3D12Resource* GetResource() const override { return m_resource.Get(); }
	ID3D12Resource* operator*() const override { return m_resource.Get(); }
	ID3D12Resource* operator->() const override { return m_resource.Get(); }
	
	ID3D12Resource* GetCounterResource() const { return m_counterResource.Get(); }

private:

	ComPtr<ID3D12Resource> m_resource;
	ComPtr<ID3D12Resource> m_counterResource;

	std::vector<T> m_cpuData;
	
	uint32_t m_uavHeapIndex = -1;
};

template <typename T>
DxStructuredBuffer<T>::DxStructuredBuffer(DxDevice& device, uint32_t numElements)
	: DxStructuredBuffer{device, std::vector(numElements, T{0})}
{}

template <typename T>
DxStructuredBuffer<T>::DxStructuredBuffer(DxDevice& device, uint32_t numElements, T value)
	: DxStructuredBuffer{device, std::vector(numElements, value)}
{}

template <typename T>
DxStructuredBuffer<T>::DxStructuredBuffer(DxDevice& device, const std::vector<T>& data)
{
	m_cpuData = data;

	// Buffer data
	const CD3DX12_HEAP_PROPERTIES heapProps{D3D12_HEAP_TYPE_DEFAULT};
	const CD3DX12_RESOURCE_DESC	  bufferDesc =
			CD3DX12_RESOURCE_DESC::Buffer(data.size() * sizeof(T), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	CheckHR(device->CreateCommittedResource(&heapProps,
											D3D12_HEAP_FLAG_NONE,
											&bufferDesc,
											D3D12_RESOURCE_STATE_COMMON,
											nullptr,
											IID_PPV_ARGS(m_resource.GetAddressOf())));

	// Counter
	const CD3DX12_RESOURCE_DESC counterBufferDesc =
			CD3DX12_RESOURCE_DESC::Buffer(sizeof(uint32_t), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	CheckHR(device->CreateCommittedResource(&heapProps,
											D3D12_HEAP_FLAG_NONE,
											&counterBufferDesc,
											D3D12_RESOURCE_STATE_COMMON,
											nullptr,
											IID_PPV_ARGS(m_counterResource.GetAddressOf())));
	
	// UAV
	m_uavHeapIndex = device.GetShaderDescriptorPile().Allocate();
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = m_cpuData.size();
	uavDesc.Buffer.StructureByteStride = sizeof(T);
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	
	device->CreateUnorderedAccessView(
		m_resource.Get(),
		m_counterResource.Get(),
		&uavDesc,
		device.GetShaderDescriptorPile().GetCpuHandleAt(m_uavHeapIndex)
	);
	
	// Upload
	DxResource::SetData(m_cpuData.data(), m_cpuData.size());
	DxResource::Upload(device.GetResourceUpload(), device.GetDXDirectComQueue(), device.GetDXDirectComList());
	
	device.GetResourceUpload().Begin();
	
	D3D12_SUBRESOURCE_DATA counterData{};
	uint32_t numElements = m_cpuData.size();
	counterData.pData = &numElements;
	counterData.RowPitch = sizeof(uint32_t);
	counterData.SlicePitch = counterData.RowPitch;
	
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_COPY_DEST);
	device.GetResourceUpload().Upload(m_counterResource.Get(), 0, &counterData, 1);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_COMMON);
	
	device.GetResourceUpload().End(device.GetDXDirectComQueue());
}
