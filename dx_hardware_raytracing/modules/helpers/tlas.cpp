module;

module dx_hw_ray.helpers.tlas;
import std;
import dx_hw_ray.external.dxr;
import dx_wrapper.helpers.dx_buffer_helpers;

void Tlas::AddBlas(const Blas& blas, const glm::mat4& transform, std::uint32_t instanceIndex, std::uint32_t hitGroupIndex)
{
	glm::mat4		  transposed = glm::transpose(transform);
	DirectX::XMMATRIX matrix{glm::value_ptr(transposed)};

	m_instances.emplace_back(*blas, matrix, instanceIndex, hitGroupIndex);
}

void Tlas::Generate(DxDevice& device)
{
	// Get the byte size of the new resources
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS prebuildDesc{};
	prebuildDesc.Type		 = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	prebuildDesc.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	prebuildDesc.NumDescs	 = m_instances.size();
	prebuildDesc.Flags		 = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;

	// Get the prebuild info
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
	device->GetRaytracingAccelerationStructurePrebuildInfo(&prebuildDesc, &info);

	// Get the buffer sizes
	const std::uint64_t scratchSize = (info.ScratchDataSizeInBytes + D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) &
									  ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);

	const std::uint64_t resultSize = (info.ResultDataMaxSizeInBytes + D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) &
									 ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);

	// Create the buffers
	ComPtr<ID3D12Resource> scratchResource;
	CheckHR(CreateStaticBuffer(device,
							   nullptr,
							   scratchSize,
							   D3D12_RESOURCE_STATE_COMMON,
							   scratchResource,
							   "TLAS Scratch Resource",
							   D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));

	CheckHR(CreateStaticBuffer(device,
							   nullptr,
							   resultSize,
							   D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
							   m_tlas,
							   "TLAS Resource",
							   D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));

	// Instances
	std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instances;
	instances.reserve(m_instances.size());

	for (const auto& instance : m_instances)
	{
		D3D12_RAYTRACING_INSTANCE_DESC instanceDesc{};
		instanceDesc.AccelerationStructure = instance.m_blas->GetGPUVirtualAddress();
		std::memcpy(instanceDesc.Transform, &instance.m_transform, sizeof(instanceDesc.Transform));
		instanceDesc.InstanceID							 = instance.m_instanceIndex;
		instanceDesc.InstanceContributionToHitGroupIndex = instance.m_hitGroupIndex;
		instanceDesc.InstanceMask						 = 0xff;
		instanceDesc.Flags								 = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		
		instances.push_back(instanceDesc);
	}

	const std::size_t instanceBufferSize =
			NextMultipleOf(sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * m_instances.size(),
						   static_cast<std::size_t>(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));

	CheckHR(CreateStaticBuffer(device,
							   instances.data(),
							   instanceBufferSize,
							   D3D12_RESOURCE_STATE_GENERIC_READ,
							   m_instanceBuffer,
							   "TLAS Instance Buffer Resource"));

	// Create the TLAS
	prebuildDesc.InstanceDescs = m_instanceBuffer->GetGPUVirtualAddress();

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc;
	buildDesc.Inputs						   = prebuildDesc;
	buildDesc.DestAccelerationStructureData	   = m_tlas->GetGPUVirtualAddress();
	buildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
	buildDesc.SourceAccelerationStructureData  = 0;

	device.GetDXDirectComList()->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
	
	device.SetUavBarrier(m_tlas.Get());
	
	device.RegisterScratchResource(std::move(scratchResource));
}
