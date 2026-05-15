module;

module dx_wrapper.rendering.dx_descriptor_pile;
import std;
import dx_wrapper.core;

DxDescriptorPile::DxDescriptorPile(ID3D12DescriptorHeap* existingHeap, const IndexType reserve)
	: DxDescriptorHeap{existingHeap}, m_top{reserve}
{
	if (reserve > 0 && m_top >= Count())
		throw std::out_of_range("Reserve descriptor range is too large");
}

DxDescriptorPile::DxDescriptorPile(ID3D12Device2* device, const D3D12_DESCRIPTOR_HEAP_DESC* desc, const IndexType reserve)
	: DxDescriptorHeap{device, desc}, m_top(reserve)
{
	if (reserve > 0 && m_top >= Count())
		throw std::out_of_range("Reserve descriptor range is too large");
}

DxDescriptorPile::DxDescriptorPile(ID3D12Device2* device, const D3D12_DESCRIPTOR_HEAP_TYPE type,
								   const D3D12_DESCRIPTOR_HEAP_FLAGS flags, const std::size_t capacity, const IndexType reserve)
	: DxDescriptorHeap{device, type, flags, capacity}, m_top{reserve}
{
	if (reserve > 0 && m_top >= Count())
		throw std::out_of_range("Reserve descriptor range is too large");
}

DxDescriptorPile::DxDescriptorPile(ID3D12Device2* device, const std::size_t count, const IndexType reserve)
	: DxDescriptorPile{
			  device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, count, reserve}
{}

DxDescriptorPile::IndexType DxDescriptorPile::Allocate()
{
	if (!m_freeIndices.empty())
	{
		const IndexType index = m_freeIndices.back();
		m_freeIndices.pop_back();
		return index;
	}

	IndexType start, end;
	AllocateRange(1, start, end);

	return start;
}

void DxDescriptorPile::AllocateRange(const std::size_t numDescriptors, IndexType& start, IndexType& end)
{
	std::ranges::sort(m_freeIndices);

	const std::size_t count = numDescriptors;
	for (std::size_t i = 0; i + count <= m_freeIndices.size(); ++i)
	{
		if (m_freeIndices[i + count - 1] == m_freeIndices[i] + count - 1)
		{
			start = m_freeIndices[i];
			end	  = start + count;
			m_freeIndices.erase(m_freeIndices.begin() + i, m_freeIndices.begin() + i + count);
			return;
		}
	}

	// make sure we didn't allocate zero
	if (numDescriptors == 0)
	{
		throw std::invalid_argument("Can't allocate zero descriptors");
	}

	// get the current top
	start = m_top;

	// increment top with new request
	m_top += numDescriptors;
	end = m_top;

	// make sure we have enough room
	if (m_top > Count())
	{
		Log::Error("DescriptorPile has %zu of %zu descriptors; failed request for %zu more\n",
							start,
							Count(),
							numDescriptors);
		throw std::runtime_error("Can't allocate more descriptors");
	}
}

void DxDescriptorPile::Free(IndexType index, const std::size_t numDescriptors)
{
	if (index == invalid) return;
	
	auto range = std::views::iota(index, index + static_cast<IndexType>(numDescriptors));
	m_freeIndices.insert(m_freeIndices.end(), range.begin(), range.end());
}
