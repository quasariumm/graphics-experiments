module;

export module dx_wrapper.rendering.dx_descriptor_heap;
import std;
import dx_wrapper.core.dx_common;
import dx_wrapper.core.log;

/**
 * A contiguous linear random-access descriptor heap
 * Code from https://github.com/microsoft/DirectXTK12/blob/main/Inc/DescriptorHeap.h
 */
export class DxDescriptorHeap
{
public:

	DxDescriptorHeap() = default;
	explicit DxDescriptorHeap(ID3D12DescriptorHeap* existingHeap);
	explicit DxDescriptorHeap(ID3D12Device5* device, const D3D12_DESCRIPTOR_HEAP_DESC* desc);
	explicit DxDescriptorHeap(ID3D12Device5* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags,
							  std::size_t count);

	explicit DxDescriptorHeap(ID3D12Device5* device, const std::size_t count)
		: DxDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, count)
	{}

	D3D12_GPU_DESCRIPTOR_HANDLE WriteDescriptors(ID3D12Device5* device, std::uint32_t offsetIntoHeap,
												 std::uint32_t						totalDescriptorCount,
												 const D3D12_CPU_DESCRIPTOR_HANDLE* descriptorRangeStarts,
												 const std::uint32_t*				descriptorRangeSizes,
												 std::uint32_t						descriptorRangeCount) const;

	D3D12_GPU_DESCRIPTOR_HANDLE WriteDescriptors(ID3D12Device5* device, std::uint32_t offsetIntoHeap,
												 const D3D12_CPU_DESCRIPTOR_HANDLE* descriptorRangeStarts,
												 const std::uint32_t*				descriptorRangeSizes,
												 std::uint32_t						descriptorRangeCount) const;

	D3D12_GPU_DESCRIPTOR_HANDLE WriteDescriptors(ID3D12Device5* device, std::uint32_t offsetIntoHeap,
												 const D3D12_CPU_DESCRIPTOR_HANDLE* pDescriptors,
												 std::uint32_t						descriptorCount) const;

	D3D12_GPU_DESCRIPTOR_HANDLE GetFirstGpuHandle() const noexcept;
	D3D12_CPU_DESCRIPTOR_HANDLE GetFirstCpuHandle() const noexcept;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandleAt(std::size_t index) const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandleAt(std::size_t index) const;

	std::size_t GetIndexFromHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) const;
	std::size_t GetIndexFromHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle) const;

	std::size_t					Count() const noexcept { return m_heapDesc.NumDescriptors; }
	D3D12_DESCRIPTOR_HEAP_FLAGS Flags() const noexcept { return m_heapDesc.Flags; }
	D3D12_DESCRIPTOR_HEAP_TYPE	Type() const noexcept { return m_heapDesc.Type; }
	std::uint32_t				Increment() const noexcept { return m_increment; }
	ID3D12DescriptorHeap*		Heap() const noexcept { return m_heap.Get(); }

	static void DefaultDesc(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_DESC* desc) noexcept;

	DxDescriptorHeap(DxDescriptorHeap&&)				 = default;
	DxDescriptorHeap& operator=(DxDescriptorHeap&&)		 = default;
	DxDescriptorHeap(const DxDescriptorHeap&)			 = delete;
	DxDescriptorHeap& operator=(const DxDescriptorHeap&) = delete;

private:

	void Create(ID3D12Device5* device, const D3D12_DESCRIPTOR_HEAP_DESC* desc);

	ComPtr<ID3D12DescriptorHeap> m_heap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC	 m_heapDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE	 m_cpuHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE	 m_gpuHandle{};
	std::uint32_t				 m_increment = 0;
};
