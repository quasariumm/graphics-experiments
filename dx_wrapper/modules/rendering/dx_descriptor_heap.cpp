module;

module dx_wrapper.rendering.dx_descriptor_heap;
import std;
import dx_wrapper.external.directx12;

// Code from https://github.com/microsoft/DirectXTK12/blob/main/Src/DescriptorHeap.cpp

DxDescriptorHeap::DxDescriptorHeap(ID3D12DescriptorHeap* existingHeap) : m_heap{existingHeap}
{
#ifdef _MSC_VER
	m_cpuHandle = existingHeap->GetCPUDescriptorHandleForHeapStart();
	m_gpuHandle = existingHeap->GetGPUDescriptorHandleForHeapStart();
	m_heapDesc	= existingHeap->GetDesc();
#else
	existingHeap->GetCPUDescriptorHandleForHeapStart(&m_cpuHandle);
	existingHeap->GetGPUDescriptorHandleForHeapStart(&m_gpuHandle);
	existingHeap->GetDesc(&m_heapDesc);
#endif

	ComPtr<ID3D12Device5> device;
	CheckHR(existingHeap->GetDevice(GetIID(device), GetPPV(device)));

	m_increment = device->GetDescriptorHandleIncrementSize(m_heapDesc.Type);
}

DxDescriptorHeap::DxDescriptorHeap(ID3D12Device5* device, const D3D12_DESCRIPTOR_HEAP_DESC* desc)
	: m_heapDesc{}, m_cpuHandle{}, m_gpuHandle{}, m_increment{0}
{ Create(device, desc); }

DxDescriptorHeap::DxDescriptorHeap(ID3D12Device5* device, const D3D12_DESCRIPTOR_HEAP_TYPE type,
								   const D3D12_DESCRIPTOR_HEAP_FLAGS flags, const std::size_t count)
	: m_heapDesc{}, m_cpuHandle{}, m_gpuHandle{}, m_increment{0}
{
	if (count > std::numeric_limits<std::uint32_t>::max())
		Log::Critical("Too many descriptors");

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags						= flags;
	desc.NumDescriptors				= static_cast<UINT>(count);
	desc.Type						= type;
	Create(device, &desc);
}

D3D12_GPU_DESCRIPTOR_HANDLE DxDescriptorHeap::WriteDescriptors(ID3D12Device5* device, const std::uint32_t offsetIntoHeap,
															   const std::uint32_t				  totalDescriptorCount,
															   const D3D12_CPU_DESCRIPTOR_HANDLE* descriptorRangeStarts,
															   const std::uint32_t*				  descriptorRangeSizes,
															   const std::uint32_t				  descriptorRangeCount) const
{
	if (static_cast<std::size_t>(offsetIntoHeap) + static_cast<std::size_t>(totalDescriptorCount) >
		static_cast<std::size_t>(m_heapDesc.NumDescriptors))
		Log::Critical("Descriptor write would go outside writable range");

	const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCpuHandleAt(offsetIntoHeap);

	device->CopyDescriptors(1,
							&cpuHandle,
							&totalDescriptorCount,
							descriptorRangeCount,
							descriptorRangeStarts,
							descriptorRangeSizes,
							m_heapDesc.Type);

	return GetGpuHandleAt(offsetIntoHeap);
}

D3D12_GPU_DESCRIPTOR_HANDLE DxDescriptorHeap::WriteDescriptors(ID3D12Device5* device, const std::uint32_t offsetIntoHeap,
															   const D3D12_CPU_DESCRIPTOR_HANDLE* descriptorRangeStarts,
															   const std::uint32_t*				  descriptorRangeSizes,
															   const std::uint32_t				  descriptorRangeCount) const
{
	std::uint32_t totalDescriptorCount = 0;
	for (std::uint32_t i = 0; i < descriptorRangeCount; ++i)
		totalDescriptorCount += descriptorRangeSizes[i];

	return WriteDescriptors(device,
							offsetIntoHeap,
							totalDescriptorCount,
							descriptorRangeStarts,
							descriptorRangeSizes,
							descriptorRangeCount);
}

D3D12_GPU_DESCRIPTOR_HANDLE DxDescriptorHeap::WriteDescriptors(ID3D12Device5* device, const std::uint32_t offsetIntoHeap,
															   const D3D12_CPU_DESCRIPTOR_HANDLE* pDescriptors,
															   const std::uint32_t				  descriptorCount) const
{ return WriteDescriptors(device, offsetIntoHeap, descriptorCount, pDescriptors, &descriptorCount, 1); }

D3D12_GPU_DESCRIPTOR_HANDLE DxDescriptorHeap::GetFirstGpuHandle() const noexcept
{
	if (!(m_heapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE))
		Log::Critical("Heap must be shader visible to get first GPU handle");
	if (m_heap == nullptr)
		Log::Critical("Heap is not initialised");
	return m_gpuHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE DxDescriptorHeap::GetFirstCpuHandle() const noexcept
{
	if (m_heap == nullptr)
		Log::Critical("Heap is not initialised");
	return m_cpuHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DxDescriptorHeap::GetGpuHandleAt(const std::size_t index) const
{
	if (m_heap == nullptr)
		Log::Critical("Heap is not initialised");
	if (index >= m_heapDesc.NumDescriptors)
		Log::Critical("Index outside of heap");
	if (!(m_heapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE))
		Log::Critical("Heap must be shader visible to get GPU handle");

	D3D12_GPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = m_gpuHandle.ptr + index * m_increment;
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE DxDescriptorHeap::GetCpuHandleAt(const std::size_t index) const
{
	if (m_heap == nullptr)
		Log::Critical("Heap is not initialised");
	if (index >= m_heapDesc.NumDescriptors)
		Log::Critical("Index outside of heap");

	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = m_cpuHandle.ptr + index * m_increment;
	return handle;
}

std::size_t DxDescriptorHeap::GetIndexFromHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) const
{ return static_cast<std::uint32_t>((handle.ptr - GetFirstCpuHandle().ptr) / m_increment); }

std::size_t DxDescriptorHeap::GetIndexFromHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle) const
{ return static_cast<std::uint32_t>((handle.ptr - GetFirstGpuHandle().ptr) / m_increment); }

void DxDescriptorHeap::DefaultDesc(const D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_DESC* desc) noexcept
{
	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
		desc->Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	else
		desc->Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc->NumDescriptors = 0;
	desc->Type			 = type;
}

void DxDescriptorHeap::Create(ID3D12Device5* device, const D3D12_DESCRIPTOR_HEAP_DESC* desc)
{
	if (!device)
		Log::Critical("No device provided");
	if (!desc)
		Log::Critical("Invalid description");

	m_heapDesc	= *desc;
	m_increment = device->GetDescriptorHandleIncrementSize(desc->Type);

	if (desc->NumDescriptors == 0)
	{
		m_heap.Reset();
		m_cpuHandle.ptr = 0;
		m_gpuHandle.ptr = 0;
	}
	else
	{
		auto** heap = m_heap.ReleaseAndGetAddressOf();
		CheckHR(device->CreateDescriptorHeap(desc, GetIID(heap), GetPPV(heap)));
		CheckHR(m_heap->SetName(L"DescriptorHeap"));

#ifdef _MSC_VER
		m_cpuHandle = m_heap->GetCPUDescriptorHandleForHeapStart();
		if (desc->Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
			m_gpuHandle = m_heap->GetGPUDescriptorHandleForHeapStart();
#else
		m_heap->GetCPUDescriptorHandleForHeapStart(&m_cpuHandle);
		if (desc->Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
			m_heap->GetGPUDescriptorHandleForHeapStart(&m_gpuHandle);
#endif
	}
}
