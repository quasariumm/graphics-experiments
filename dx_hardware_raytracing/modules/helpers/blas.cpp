module;

module dx_hw_ray.helpers.blas;
import std;
import dx_hw_ray.external.dxr;
import dx_wrapper.helpers.dx_buffer_helpers;

void Blas::AddGeometry(const DxDevice& device, const glm::mat4& transform, ID3D12Resource* vertexBuffer, std::size_t vertexStride,
					   std::uint32_t vertexCount, ID3D12Resource* indexBuffer, std::uint32_t indexCount,
					   DXGI_FORMAT indexFormat, D3D12_RAYTRACING_GEOMETRY_FLAGS additionalFlags)
{
	// Create a geometry description
	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc{};
	geometryDesc.Type								  = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Triangles.VertexBuffer.StartAddress  = vertexBuffer->GetGPUVirtualAddress();
	geometryDesc.Triangles.VertexBuffer.StrideInBytes = vertexStride;
	geometryDesc.Triangles.VertexCount				  = vertexCount;
	geometryDesc.Triangles.VertexFormat				  = DXGI_FORMAT_R32G32B32_FLOAT;

	geometryDesc.Triangles.IndexBuffer = indexBuffer->GetGPUVirtualAddress();
	geometryDesc.Triangles.IndexFormat = indexFormat;
	geometryDesc.Triangles.IndexCount  = indexCount;

	const glm::mat4 transposed = glm::transpose(transform);

	ComPtr<ID3D12Resource> transformBuffer;
	CheckHR(CreateStaticBuffer(device,
							   glm::value_ptr(transposed),
							   sizeof(glm::mat3x4),
							   D3D12_RESOURCE_STATE_COMMON,
							   transformBuffer,
							   "BLAS geometry transform buffer"));

	geometryDesc.Triangles.Transform3x4 = transformBuffer->GetGPUVirtualAddress();
	m_transforms.emplace_back(std::move(transformBuffer));

	geometryDesc.Flags = additionalFlags;

	m_geometries.push_back(std::move(geometryDesc));
}

void Blas::Generate(DxDevice& device)
{
	// Get the byte size of the new resources
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS prebuildDesc{};
	prebuildDesc.Type			= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	prebuildDesc.DescsLayout	= D3D12_ELEMENTS_LAYOUT_ARRAY;
	prebuildDesc.NumDescs		= m_geometries.size();
	prebuildDesc.pGeometryDescs = m_geometries.data();
	prebuildDesc.Flags			= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	// Get the prebuild info
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
	device->GetRaytracingAccelerationStructurePrebuildInfo(&prebuildDesc, &info);

	// Get the buffer sizes
	const std::uint64_t scratchSize =
			NextMultipleOf(info.ScratchDataSizeInBytes,
						   static_cast<std::uint64_t>(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));

	const std::uint64_t resultSize = NextMultipleOf(info.ResultDataMaxSizeInBytes,
													static_cast<std::uint64_t>(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));

	// Create the buffers
	ComPtr<ID3D12Resource> scratchResource;
	CheckHR(CreateStaticBuffer(device,
							   nullptr,
							   scratchSize,
							   D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
							   scratchResource,
							   "BLAS Scratch Resource",
							   D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));

	CheckHR(CreateStaticBuffer(device,
							   nullptr,
							   resultSize,
							   D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
							   m_blas,
							   "BLAS Resource",
							   D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));

	// Generate the BLAS
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc{};
	buildDesc.Inputs						   = prebuildDesc;
	buildDesc.DestAccelerationStructureData	   = m_blas->GetGPUVirtualAddress();
	buildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
	buildDesc.SourceAccelerationStructureData  = 0;

	// Build the BLAS
	device.GetDXDirectComList()->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);

	device.SetUavBarrier(m_blas.Get());

	device.RegisterScratchResource(std::move(scratchResource));
}
