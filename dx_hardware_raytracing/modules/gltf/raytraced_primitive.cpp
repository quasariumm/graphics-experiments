module;

module dx_hw_ray.gltf.raytraced_primitive;
import dx_hw_ray.external.dxr;
import dx_wrapper.resources.dx_resource;
import dx_wrapper.helpers.dx_buffer_helpers;

RaytracedGltfPrimitive::RaytracedGltfPrimitive(DxDevice& device, const std::filesystem::path& modelPath,
											   const fastgltf::Asset& asset, const fastgltf::Primitive& primitive)
	: GltfPrimitive{device, modelPath, asset, primitive}
{
	D3D12_RAYTRACING_GEOMETRY_FLAGS extraFlags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
	if (!m_material || m_material->m_alphaMode == fastgltf::AlphaMode::Opaque)
		extraFlags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	// Set geometry buffers to NON_PIXEL_RESOURCE because it cannot be in the states normal to rasterisation
	Transition(device.GetDXDirectComList(),
			   m_vertexBuffer.Get(),
			   D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	
	Transition(device.GetDXDirectComList(),
			   m_indexBuffer.Get(),
			   D3D12_RESOURCE_STATE_INDEX_BUFFER,
			   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	m_blas.GenerateTriangles(device,
							 m_vertexBuffer.Get(),
							 sizeof(Vertex),
							 m_vertices.size(),
							 m_indexBuffer.Get(),
							 m_indices.size(),
							 DXGI_FORMAT_R32_UINT,
							 extraFlags);
}
