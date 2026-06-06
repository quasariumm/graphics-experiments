module;

module dx_hw_ray.gltf.scene_geometry_buffer;
import dx_wrapper.core.dx_common;
import dx_wrapper.helpers.dx_buffer_helpers;
import dx_wrapper.helpers.dx_format_helpers;

SceneGeometryBuffer::~SceneGeometryBuffer()
{
	if (!m_device) return;
	m_device->GetShaderDescriptorPile().Free(m_vertexHeapIndex, m_lastSize);
	m_device->GetShaderDescriptorPile().Free(m_indexHeapIndex, m_lastSize);
	m_device->GetShaderDescriptorPile().Free(m_materialsHeapIndex, m_materialsBuffers.size());
	m_device->GetShaderDescriptorPile().Free(m_materialIndicesHeapIndex, m_materialIndexBuffers.size());
	m_device->GetShaderDescriptorPile().Free(m_blasGeometryCountHeapIndex);
}

void SceneGeometryBuffer::AddPrimitive(const ComPtr<ID3D12Resource>& vertexBuffer, const ComPtr<ID3D12Resource>& indexBuffer)
{
	m_vertexBuffers.push_back(vertexBuffer);
	m_indexBuffers.push_back(indexBuffer);
}

void SceneGeometryBuffer::AddModel(const RaytracedModel& model)
{
	// Add material buffers
	m_materialsBuffers.push_back(model.GetMaterialList());
	m_materialIndexBuffers.push_back(model.GetMaterialIndicesList());

	// Add primitives
	std::uint32_t geometryCount = 0;
	for (const auto& node : model.GetNodes())
	{
		if (!node->m_mesh)
			continue;

		for (const auto& primitive : node->m_mesh->m_primitives)
		{
			AddPrimitive(primitive.GetVertexBuffer(), primitive.GetIndexBuffer());
			geometryCount++;
		}
	}

	m_blasGeometryCounts.push_back(geometryCount);
}

struct ShaderMaterial
{
	float m_alphaCutoff = 0.5f;

	glm::vec3 m_emissiveFactor{1.f};
	glm::vec4 m_baseColorFactor{1.f};

	float m_metallicFactor	  = 0.0f;
	float m_roughnessFactor	  = 1.0f;
	float m_normalScale		  = 1.0f;
	float m_occlusionStrength = 1.0f;

	int m_texIndices[8]{-1};

	std::uint32_t m_flags = 0;
	std::uint32_t m_padding[3];
};

void SceneGeometryBuffer::Generate(DxDevice& device)
{
	m_device = &device;
	// Cleanup
	if (m_vertexHeapIndex != DxDescriptorPile::invalid)
	{
		device.GetShaderDescriptorPile().Free(m_vertexHeapIndex, m_lastSize);
		device.GetShaderDescriptorPile().Free(m_indexHeapIndex, m_lastSize);
		device.GetShaderDescriptorPile().Free(m_materialsHeapIndex, m_materialsBuffers.size());
		device.GetShaderDescriptorPile().Free(m_materialIndicesHeapIndex, m_materialIndexBuffers.size());
		device.GetShaderDescriptorPile().Free(m_blasGeometryCountHeapIndex);
	}
	
	m_lastSize = m_vertexBuffers.size();

	// Allocate
	DxDescriptorPile::IndexType vertexEnd;
	device.GetShaderDescriptorPile().AllocateRange(m_vertexBuffers.size(), m_vertexHeapIndex, vertexEnd);

	DxDescriptorPile::IndexType indexEnd;
	device.GetShaderDescriptorPile().AllocateRange(m_indexBuffers.size(), m_indexHeapIndex, indexEnd);

	DxDescriptorPile::IndexType materialsEnd;
	device.GetShaderDescriptorPile().AllocateRange(m_materialsBuffers.size(), m_materialsHeapIndex, materialsEnd);

	DxDescriptorPile::IndexType materialIndicesEnd;
	device.GetShaderDescriptorPile().AllocateRange(m_materialIndexBuffers.size(),
												   m_materialIndicesHeapIndex,
												   materialIndicesEnd);

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

	// Material lists
	for (auto i = m_materialsHeapIndex; i < materialsEnd; ++i)
	{
		CreateShaderResourceView(device,
								 device.GetShaderDescriptorPile(),
								 i,
								 m_materialsBuffers[i - m_materialsHeapIndex].Get(),
								 sizeof(ShaderMaterial));
	}

	for (auto i = m_materialIndicesHeapIndex; i < materialIndicesEnd; ++i)
	{
		CreateShaderResourceView(device,
								 device.GetShaderDescriptorPile(),
								 i,
								 m_materialIndexBuffers[i - m_materialIndicesHeapIndex].Get());
	}

	// Geometry counts
	CheckHR(CreateStaticBuffer(device,
							   m_blasGeometryCounts,
							   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
							   m_blasGeometryCountBuffer,
							   "BLAS Geometry Counts Buffer"));
	m_blasGeometryCountHeapIndex = device.GetShaderDescriptorPile().Allocate();

	CreateShaderResourceView(device,
							 device.GetShaderDescriptorPile(),
							 m_blasGeometryCountHeapIndex,
							 m_blasGeometryCountBuffer.Get());
}
