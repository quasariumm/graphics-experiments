module;

export module dx_wrapper.resources.dx_structured_buffer;
import std;
import dx_wrapper.resources.dx_resource;
import dx_wrapper.core;
import dx_wrapper.rendering.dx_descriptor_pile;
import dx_wrapper.helpers.dx_buffer_helpers;

export template <typename T>
class DxStructuredBuffer : protected DxResource
{

public:

	DxStructuredBuffer() = default;

	explicit DxStructuredBuffer(DxDevice& device, std::uint32_t numElements, const std::string& name = "");
	explicit DxStructuredBuffer(DxDevice& device, std::uint32_t numElements, T value, const std::string& name = "");
	explicit DxStructuredBuffer(DxDevice& device, const std::vector<T>& data, const std::string& name = "");

	ID3D12Resource* GetResource() const override { return m_resource.Get(); }
	ID3D12Resource* operator*() const override { return m_resource.Get(); }
	ID3D12Resource* operator->() const override { return m_resource.Get(); }

	ID3D12Resource* GetCounterResource() const { return m_counterResource.Get(); }
	
	DxDescriptorPile::IndexType GetSrvHeapIndex() const { return m_srvHeapIndex; }
	DxDescriptorPile::IndexType GetUavHeapIndex() const { return m_uavHeapIndex; }

private:

	ComPtr<ID3D12Resource> m_resource;
	ComPtr<ID3D12Resource> m_counterResource;

	std::vector<T> m_cpuData;

	DxDescriptorPile::IndexType m_srvHeapIndex = -1;
	DxDescriptorPile::IndexType m_uavHeapIndex = -1;
};

template <typename T>
DxStructuredBuffer<T>::DxStructuredBuffer(DxDevice& device, std::uint32_t numElements, const std::string& name)
	: DxStructuredBuffer{device, std::vector(numElements, T{})}
{}

template <typename T>
DxStructuredBuffer<T>::DxStructuredBuffer(DxDevice& device, std::uint32_t numElements, T value, const std::string& name)
	: DxStructuredBuffer{device, std::vector(numElements, value)}
{}

template <typename T>
DxStructuredBuffer<T>::DxStructuredBuffer(DxDevice& device, const std::vector<T>& data, const std::string& name)
{
	m_cpuData = data;

	// Buffer data
	const CD3DX12_HEAP_PROPERTIES heapProps{D3D12_HEAP_TYPE_DEFAULT};
	const CD3DX12_RESOURCE_DESC	  bufferDesc =
			CD3DX12_RESOURCE_DESC::Buffer(data.size() * sizeof(T), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	const std::wstring wname = std::filesystem::path(name).wstring();
	CheckHR(device->CreateCommittedResource(&heapProps,
											D3D12_HEAP_FLAG_NONE,
											&bufferDesc,
											D3D12_RESOURCE_STATE_COMMON,
											nullptr,
											GetIID(m_resource),
											GetPPV(m_resource)));
	CheckHR(m_resource->SetName(wname.c_str()));

	// Counter
	const CD3DX12_RESOURCE_DESC counterBufferDesc =
			CD3DX12_RESOURCE_DESC::Buffer(sizeof(std::uint32_t), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	const auto counterName = wname + L" (Counter)";
	CheckHR(device->CreateCommittedResource(&heapProps,
											D3D12_HEAP_FLAG_NONE,
											&counterBufferDesc,
											D3D12_RESOURCE_STATE_COMMON,
											nullptr,
											GetIID(m_counterResource),
											GetPPV(m_counterResource)));
	CheckHR(m_counterResource->SetName(counterName.c_str()));

	// Descriptors
	m_srvHeapIndex = AllocateShaderResourceView(device, m_resource.Get(), sizeof(T));
	m_uavHeapIndex = AllocateUnorderedAccessView(device, m_resource.Get(), sizeof(T), m_counterResource.Get());

	// Upload
	DxResource::SetData(m_cpuData.data(), sizeof(T) * m_cpuData.size());
	DxResource::Upload(device.GetResourceUpload(), device.GetDXDirectComQueue(), device.GetDXDirectComList());

	device.GetResourceUpload().Begin();

	D3D12_SUBRESOURCE_DATA counterData{};
	std::uint32_t		   numElements = m_cpuData.size();
	counterData.pData				   = &numElements;
	counterData.RowPitch			   = sizeof(std::uint32_t);
	counterData.SlicePitch			   = counterData.RowPitch;

	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_COPY_DEST);
	device.GetResourceUpload().Upload(m_counterResource.Get(), 0, &counterData, 1);
	Transition(device.GetDXDirectComList(), D3D12_RESOURCE_STATE_COMMON);

	device.GetResourceUpload().End(device.GetDXDirectComQueue()).wait();
}
