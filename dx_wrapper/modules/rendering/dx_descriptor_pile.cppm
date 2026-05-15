module;

export module dx_wrapper.rendering.dx_descriptor_pile;
import std;
import dx_wrapper.external.directx12;
import dx_wrapper.rendering.dx_descriptor_heap;

/**
 * A class that stores a set number of descriptors for use in shaders.
 */
export class DxDescriptorPile : public DxDescriptorHeap
{

public:

	using IndexType					   = std::int32_t;
	static constexpr IndexType invalid = IndexType{-1};

	explicit DxDescriptorPile(ID3D12DescriptorHeap* existingHeap, IndexType reserve = 0);
	explicit DxDescriptorPile(ID3D12Device2* device, const D3D12_DESCRIPTOR_HEAP_DESC* desc, IndexType reserve = 0);
	explicit DxDescriptorPile(ID3D12Device2* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags,
							  std::size_t capacity, IndexType reserve = 0);
	explicit DxDescriptorPile(ID3D12Device2* device, std::size_t count, IndexType reserve = 0);

	ID3D12DescriptorHeap* GetHeap() const noexcept { return Heap(); }
	ID3D12DescriptorHeap* operator*() const noexcept { return Heap(); }
	ID3D12DescriptorHeap* operator->() const noexcept { return Heap(); }

	IndexType Allocate();
	void	  AllocateRange(std::size_t numDescriptors, IndexType& start, IndexType& end);
	void	  Free(IndexType index, std::size_t numDescriptors = 1);

	DxDescriptorPile(const DxDescriptorPile&)			 = delete;
	DxDescriptorPile& operator=(const DxDescriptorPile&) = delete;
	DxDescriptorPile(DxDescriptorPile&&)				 = default;
	DxDescriptorPile& operator=(DxDescriptorPile&&)		 = default;

private:

	IndexType m_top{0};

	std::vector<IndexType> m_freeIndices{};
};
