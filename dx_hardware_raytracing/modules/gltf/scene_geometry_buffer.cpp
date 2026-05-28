module;

module dx_hw_ray.gltf.scene_geometry_buffer;
import dx_wrapper.core.dx_common;
import dx_wrapper.helpers.dx_buffer_helpers;
import dx_wrapper.helpers.dx_format_helpers;

SceneGeometryBuffer::~SceneGeometryBuffer() {}

void SceneGeometryBuffer::AddPrimitive(const ComPtr<ID3D12Resource>& vertexBuffer, const ComPtr<ID3D12Resource>& indexBuffer)
{
	m_vertexBuffers.push_back(vertexBuffer);
	m_indexBuffers.push_back(indexBuffer);
}

void SceneGeometryBuffer::Generate(DxDevice& device)
{
	// Cleanup
	if (m_vertexHeapIndex != DxDescriptorPile::invalid)
	{
		device.GetShaderDescriptorPile().Free(m_vertexHeapIndex, m_lastSize);
		device.GetShaderDescriptorPile().Free(m_indexHeapIndex, m_lastSize);
	}

	// Allocate
	DxDescriptorPile::IndexType vertexEnd;
	device.GetShaderDescriptorPile().AllocateRange(m_vertexBuffers.size(), m_vertexHeapIndex, vertexEnd);

	DxDescriptorPile::IndexType indexEnd;
	device.GetShaderDescriptorPile().AllocateRange(m_indexBuffers.size(), m_indexHeapIndex, indexEnd);

	// Fill with SRVs
	for (auto i = m_vertexHeapIndex; i < vertexEnd; ++i)
	{
		CreateShaderResourceView(device,
								 device.GetShaderDescriptorPile(),
								 i,
								 m_vertexBuffers[i - m_vertexHeapIndex].Get(),
								 sizeof(Vertex));
	}

	for (auto i = m_indexHeapIndex; i < indexEnd; ++i)
	{
		CreateShaderResourceView(device,
								 device.GetShaderDescriptorPile(),
								 i,
								 m_indexBuffers[i - m_indexHeapIndex].Get(),
								 sizeof(std::uint32_t));
	}
}
