module;

module dx_hw_ray.rendering.dx_ray_pipeline;
import dx_wrapper.core.dx_common;
import dx_wrapper.core.log;
import dx_wrapper.rendering.dx_shader_utils;
import dx_wrapper.helpers.dx_buffer_helpers;

// Code mostly from here:
// https://github.com/ScrappyCocco/DirectX-DXR-Tutorials/blob/master/01-Dx12DXRTriangle/Project/nv_helpers_dx12/RaytracingPipelineGenerator.cpp
// (cleaned up to fit my wrapper and coding standards)

DxRayPipeline& DxRayPipeline::SetRayGenShader(const std::filesystem::path& path, const std::string& entryPoint,
											  const std::vector<void*>& inputs)
{
	AddLibrary(path, {entryPoint});
	m_rayGenName   = std::filesystem::path{entryPoint}.wstring();
	m_rayGenInputs = inputs;
	return *this;
}

DxRayPipeline& DxRayPipeline::AddMissShader(const std::filesystem::path& path, const std::string& entryPoint,
											const std::vector<void*>& inputs)
{
	AddLibrary(path, {entryPoint});
	m_missNames.push_back(std::filesystem::path{entryPoint}.wstring());
	m_missInputs.emplace_back(inputs);
	return *this;
}

DxRayPipeline& DxRayPipeline::AddHitGroup(const std::string& hitGroupName, const std::filesystem::path& shaderPath,
										  const std::string& closestHitEntryPoint, const std::string& anyHitEntryPoint,
										  const std::string& intersectEntryPoint, const std::vector<void*>& inputs)
{
	AddLibrary(shaderPath, {closestHitEntryPoint});
	if (!anyHitEntryPoint.empty())
		AddLibrary(shaderPath, {anyHitEntryPoint});
	if (!intersectEntryPoint.empty())
		AddLibrary(shaderPath, {intersectEntryPoint});

	HitGroup hitGroup{};
	hitGroup.m_inputs	  = inputs;
	hitGroup.m_name		  = std::filesystem::path{hitGroupName}.wstring();
	hitGroup.m_closestHit = std::filesystem::path{closestHitEntryPoint}.wstring();
	hitGroup.m_anyHit	  = std::filesystem::path{anyHitEntryPoint}.wstring();
	hitGroup.m_intersect  = std::filesystem::path{intersectEntryPoint}.wstring();

	D3D12_HIT_GROUP_DESC desc{};
	desc.Type					  = D3D12_HIT_GROUP_TYPE_TRIANGLES;
	desc.HitGroupExport			  = hitGroup.m_name.c_str();
	desc.ClosestHitShaderImport	  = hitGroup.m_closestHit.c_str();
	desc.AnyHitShaderImport		  = (anyHitEntryPoint.empty()) ? nullptr : hitGroup.m_anyHit.c_str();
	desc.IntersectionShaderImport = (intersectEntryPoint.empty()) ? nullptr : hitGroup.m_intersect.c_str();

	hitGroup.m_hitDesc = desc;

	m_hitGroups.emplace_back(hitGroup);

	return *this;
}

DxRayPipeline& DxRayPipeline::AddRootSignatureAssociation(const DxRootSignature&		  rootSignature,
														  const std::vector<std::string>& entryPoints)
{
	RootSignatureAssociation association{};
	association.m_rootSignature		   = *rootSignature;
	association.m_rootSignaturePointer = *rootSignature;

	association.m_symbols.reserve(entryPoints.size());
	association.m_symbolPointers.reserve(entryPoints.size());

	for (const auto& entryPoint : entryPoints)
		association.m_symbols.emplace_back(std::filesystem::path{entryPoint}.wstring());

	for (const auto& symbol : association.m_symbols)
		association.m_symbolPointers.emplace_back(symbol.c_str());

	association.m_association.NumExports = association.m_symbolPointers.size();
	association.m_association.pExports	 = association.m_symbolPointers.data();

	m_rootSignatureAssociations.emplace_back(association);

	return *this;
}

DxRayPipeline& DxRayPipeline::SetMaxPayloadSize(const std::size_t byteSize)
{
	m_maxPayloadSizeInBytes = byteSize;
	return *this;
}

DxRayPipeline& DxRayPipeline::SetMaxAttributesSize(const std::size_t byteSize)
{
	m_maxAttributeSizeInBytes = byteSize;
	return *this;
}

DxRayPipeline& DxRayPipeline::SetMaxRecursionDepth(const std::size_t depth)
{
	m_maxRecursionDepth = depth;
	return *this;
}

void DxRayPipeline::Finalize(DxDevice& device, const std::string& name)
{
	CreateDummyRootSignatures(device);

	CreateStateObject(device);

	const std::wstring wName = std::filesystem::path{name}.wstring();
	CheckHR(m_stateObject->SetName(wName.c_str()));

	ConfigureStateObjectProperties(device);
}

std::uint32_t DxRayPipeline::GetRayGenEntrySize() const { return m_rayGenEntrySize; }
std::uint32_t DxRayPipeline::GetRayGenSectionSize() const { return m_rayGenEntrySize; }
std::uint32_t DxRayPipeline::GetMissEntrySize() const { return m_missEntrySize; }
std::uint32_t DxRayPipeline::GetMissSectionSize() const { return m_missEntrySize * m_missNames.size(); }
std::uint32_t DxRayPipeline::GetHitGroupEntrySize() const { return m_hitGroupEntrySize; }
std::uint32_t DxRayPipeline::GetHitGroupSectionSize() const { return m_hitGroupEntrySize * m_hitGroups.size(); }

void DxRayPipeline::AddLibrary(const std::filesystem::path& path, const std::vector<std::string>& entryPoints)
{
	auto& library = m_libraries.try_emplace(path, Library{}).first->second;

	if (library.m_blob == nullptr)
	{
		const auto res = RuntimeCompileShader(path, "lib", "6_6");
		if (res)
			library.m_blob = std::move(*res);
	}

	for (const auto& entryPoint : entryPoints)
	{
		std::wstring wEndpoint = std::filesystem::path{entryPoint}.wstring();
		library.m_entryPoints.emplace_back(std::move(wEndpoint));
	}

	// Rebuild because of re-allocation
	library.m_exports.clear();
	library.m_exports.reserve(library.m_entryPoints.size());
	for (const auto& entryPoint : library.m_entryPoints)
	{
		D3D12_EXPORT_DESC exportDesc;
		exportDesc.Flags		  = D3D12_EXPORT_FLAG_NONE;
		exportDesc.Name			  = entryPoint.c_str();
		exportDesc.ExportToRename = nullptr;

		library.m_exports.emplace_back(exportDesc);
	}

	library.m_dxilLibDesc.DXILLibrary =
			CD3DX12_SHADER_BYTECODE{library.m_blob->GetBufferPointer(), library.m_blob->GetBufferSize()};
	library.m_dxilLibDesc.NumExports = library.m_exports.size();
	library.m_dxilLibDesc.pExports	 = library.m_exports.data();
}

void DxRayPipeline::CreateStateObject(const DxDevice& device)
{
	const std::size_t subobjectCount = m_libraries.size()						// DXIL libraries
									   + m_hitGroups.size()						// HitGroup associations
									   + 1										// Shader config
									   + 1										// Shader payload
									   + 2 * m_rootSignatureAssociations.size() // Root sig declaration + association
									   + 2										// Dummy global and local root sig
									   + 1;										// Final pipeline object

	std::vector<D3D12_STATE_SUBOBJECT> subobjects;
	subobjects.reserve(subobjectCount);

	// Add libraries
	for (const auto& library : m_libraries | std::views::values)
	{
		D3D12_STATE_SUBOBJECT subobject{};
		subobject.Type	= D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
		subobject.pDesc = &library.m_dxilLibDesc;
		subobjects.push_back(subobject);
	}

	// Add hit groups
	for (const auto& hitGroup : m_hitGroups)
	{
		D3D12_STATE_SUBOBJECT subobject{};
		subobject.Type	= D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
		subobject.pDesc = &hitGroup.m_hitDesc;
		subobjects.push_back(subobject);
	}

	// Shader config
	D3D12_RAYTRACING_SHADER_CONFIG shaderConfig{};
	shaderConfig.MaxPayloadSizeInBytes	 = m_maxPayloadSizeInBytes;
	shaderConfig.MaxAttributeSizeInBytes = m_maxAttributeSizeInBytes;

	D3D12_STATE_SUBOBJECT shaderConfigSubobject{};
	shaderConfigSubobject.Type	= D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
	shaderConfigSubobject.pDesc = &shaderConfig;

	subobjects.push_back(shaderConfigSubobject);

	// Exported symbols
	std::unordered_set<std::wstring> exports;
	std::vector<LPCWSTR>			 exportedSymbolPointers{};
	BuildExportList(exports, exportedSymbolPointers);

	D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION shaderPayloadAssociation{};
	shaderPayloadAssociation.NumExports			   = exportedSymbolPointers.size();
	shaderPayloadAssociation.pExports			   = exportedSymbolPointers.data();
	shaderPayloadAssociation.pSubobjectToAssociate = &subobjects[subobjects.size() - 1];

	D3D12_STATE_SUBOBJECT shaderPayload{};
	shaderPayload.Type	= D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
	shaderPayload.pDesc = &shaderPayloadAssociation;
	subobjects.push_back(shaderPayload);

	// Root signature associations
	for (auto& rootSigAssociation : m_rootSignatureAssociations)
	{
		// One for the declaration
		D3D12_STATE_SUBOBJECT declaration{};
		declaration.Type  = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
		declaration.pDesc = &rootSigAssociation.m_rootSignature;

		subobjects.push_back(declaration);

		// And one for the association
		rootSigAssociation.m_association.pSubobjectToAssociate = &subobjects[subobjects.size() - 1];
		D3D12_STATE_SUBOBJECT association{};
		association.Type  = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		association.pDesc = &rootSigAssociation.m_association;

		subobjects.push_back(association);
	}

	// Dummy root sigs
	D3D12_GLOBAL_ROOT_SIGNATURE globalRootSignature{};
	globalRootSignature.pGlobalRootSignature = *m_dummyGlobalRootSignature;
	{
		D3D12_STATE_SUBOBJECT subobject{};
		subobject.Type	= D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
		subobject.pDesc = &globalRootSignature;
		subobjects.push_back(subobject);
	}

	D3D12_LOCAL_ROOT_SIGNATURE localRootSignature{};
	localRootSignature.pLocalRootSignature = *m_dummyLocalRootSignature;
	{
		D3D12_STATE_SUBOBJECT subobject{};
		subobject.Type	= D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
		subobject.pDesc = &localRootSignature;
		subobjects.push_back(subobject);
	}

	// Pipeline config
	D3D12_RAYTRACING_PIPELINE_CONFIG pipelineConfig{};
	pipelineConfig.MaxTraceRecursionDepth = m_maxRecursionDepth;

	D3D12_STATE_SUBOBJECT pipelineConfigSubobject = {};
	pipelineConfigSubobject.Type				  = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
	pipelineConfigSubobject.pDesc				  = &pipelineConfig;
	subobjects.push_back(pipelineConfigSubobject);

	// Make the final state object
	D3D12_STATE_OBJECT_DESC pipelineDesc = {};
	pipelineDesc.Type					 = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	pipelineDesc.NumSubobjects			 = static_cast<UINT>(subobjects.size());
	pipelineDesc.pSubobjects			 = subobjects.data();

	CheckHR(device->CreateStateObject(&pipelineDesc, GetIID(&m_stateObject), GetPPV(&m_stateObject)));

	CheckHR(m_stateObject->QueryInterface(GetIID(&m_stateObjectProperties), GetPPV(&m_stateObjectProperties)));
}

inline void CopyShaderData(ID3D12StateObjectProperties* properties, const std::wstring& entryPoint,
						   const std::vector<void*>& inputs, std::byte*& outData, const std::uint32_t entrySize)
{
	// Get the shader identifier, and check whether that identifier is known
	void* id = properties->GetShaderIdentifier(entryPoint.c_str());
	if (!id)
	{
		std::string strEntryPoint = std::filesystem::path{entryPoint}.string();
		Log::Critical("Unknown shader entry point used in Shader Binding Table: {}", strEntryPoint);
	}
	// Copy the shader identifier
	std::memcpy(outData, id, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	// Copy all its resources pointers or values in bulk
	std::memcpy(outData + D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES, inputs.data(), inputs.size() * 8);

	outData += entrySize;
}

void DxRayPipeline::ConfigureStateObjectProperties(DxDevice& device)
{
	// Compute the entry size of each program type depending on the maximum number of parameters in
	// each category
	{
		// RayGen group entry size
		const std::size_t maxArgs = std::max(0uz, m_rayGenInputs.size());

		std::uint32_t entrySize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		entrySize += 8 * static_cast<std::uint32_t>(maxArgs);

		m_rayGenEntrySize = NextMultipleOf(entrySize, static_cast<std::uint32_t>(D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT));
	}

	// Miss group entry size
	{
		std::size_t maxArgs = 0;
		for (const auto& inputs : m_missInputs)
			maxArgs = std::max(maxArgs, inputs.size());

		std::uint32_t entrySize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		entrySize += 8 * static_cast<std::uint32_t>(maxArgs);

		m_missEntrySize = NextMultipleOf(entrySize, static_cast<std::uint32_t>(D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT));
	}

	// Hit group entry size
	{
		std::size_t maxArgs = 0;
		for (const auto& hitGroup : m_hitGroups)
			maxArgs = std::max(maxArgs, hitGroup.m_inputs.size());

		std::uint32_t entrySize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		entrySize += 8 * static_cast<std::uint32_t>(maxArgs);

		m_hitGroupEntrySize =
				NextMultipleOf(entrySize, static_cast<std::uint32_t>(D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT));
	}

	// The total SBT size is the sum of the entries for ray generation, miss and hit groups, aligned
	// on 256 bytes
	const std::uint32_t sbtSize =
			NextMultipleOf(m_rayGenEntrySize + m_missEntrySize * static_cast<std::uint32_t>(m_missNames.size()) +
								   m_hitGroupEntrySize * static_cast<std::uint32_t>(m_hitGroups.size()),
						   256u);

	auto* cpuData = new std::byte[sbtSize];

	std::byte* walkingPointer = cpuData;

	CopyShaderData(m_stateObjectProperties, m_rayGenName, m_rayGenInputs, walkingPointer, m_rayGenEntrySize);

	for (int i = 0; i < m_missNames.size(); ++i)
		CopyShaderData(m_stateObjectProperties, m_missNames[i], m_missInputs[i], walkingPointer, m_missEntrySize);

	for (const auto& hitGroup : m_hitGroups)
		CopyShaderData(m_stateObjectProperties, hitGroup.m_name, hitGroup.m_inputs, walkingPointer, m_hitGroupEntrySize);

	CheckHR(CreateStaticBuffer(device, cpuData, sbtSize, D3D12_RESOURCE_STATE_COMMON, m_sbtResource));

	delete[] cpuData;
}

void DxRayPipeline::CreateDummyRootSignatures(DxDevice& device)
{
	m_dummyGlobalRootSignature = DxRootSignature{};
	m_dummyGlobalRootSignature.Finalize(device, "Ray Dummy Global Root Signature", true);

	m_dummyLocalRootSignature = DxRootSignature{};
	m_dummyLocalRootSignature.Finalize(device,
									   "Ray Dummy Local Root Signature",
									   false,
									   D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);
}

void DxRayPipeline::BuildExportList(std::unordered_set<std::wstring>& exports, std::vector<LPCWSTR>& exportList)
{
	// Add all the symbols exported by the libraries
	for (const Library& lib : m_libraries | std::views::values)
	{
		for (const auto& exportName : lib.m_entryPoints)
		{
#ifndef NDEBUG
			// Sanity check in debug mode: check that no name is exported more than once
			if (exports.contains(exportName))
				Log::Critical("Multiple definition of a symbol in the imported DXIL libraries");
#endif
			exports.insert(exportName);
		}
	}

#ifndef NDEBUG
	// Sanity check in debug mode: verify that the hit groups do not reference an unknown shader name
	std::unordered_set<std::wstring> all_exports = exports;

	for (const auto& hitGroup : m_hitGroups)
	{
		if (!hitGroup.m_anyHit.empty() && !exports.contains(hitGroup.m_anyHit))
			Log::Critical("Any hit symbol not found in the imported DXIL libraries");

		if (!hitGroup.m_closestHit.empty() && !exports.contains(hitGroup.m_closestHit))
			Log::Critical("Closest hit symbol not found in the imported DXIL libraries");

		if (!hitGroup.m_intersect.empty() && !exports.contains(hitGroup.m_intersect))
			Log::Critical("Intersection symbol not found in the imported DXIL libraries");

		all_exports.insert(hitGroup.m_name);
	}

	// Sanity check in debug mode: verify that the root signature associations do not reference an
	// unknown shader or hit group name
	for (const auto& assoc : m_rootSignatureAssociations)
	{
		for (const auto& symb : assoc.m_symbols)
		{
			if (!symb.empty() && !all_exports.contains(symb))
				throw std::logic_error("Root association symbol not found in the "
									   "imported DXIL libraries and hit group names");
		}
	}
#endif

	// Remove all hit group related entries from the list
	for (const auto& hitGroup : m_hitGroups)
	{
		if (!hitGroup.m_anyHit.empty() && exports.contains(hitGroup.m_anyHit))
			exports.erase(hitGroup.m_anyHit);

		if (!hitGroup.m_closestHit.empty() && exports.contains(hitGroup.m_closestHit))
			exports.erase(hitGroup.m_closestHit);

		if (!hitGroup.m_intersect.empty() && exports.contains(hitGroup.m_intersect))
			exports.erase(hitGroup.m_intersect);

		exports.insert(hitGroup.m_name);
	}

	// Build the export list
	exportList.reserve(exports.size());
	for (const auto& exportStr : exports)
		exportList.emplace_back(exportStr.c_str());
}

DxRayPipeline::Library& DxRayPipeline::Library::operator=(const Library& rhs)
{
	if (!rhs.m_blob)
		return *this;

	m_blob		  = rhs.m_blob;
	m_entryPoints = rhs.m_entryPoints;

	// Rebuild exports
	m_exports.clear();
	m_exports.reserve(m_entryPoints.size());
	for (const auto& entryPoint : m_entryPoints)
	{
		D3D12_EXPORT_DESC exportDesc;
		exportDesc.Flags		  = D3D12_EXPORT_FLAG_NONE;
		exportDesc.Name			  = entryPoint.c_str();
		exportDesc.ExportToRename = nullptr;

		m_exports.emplace_back(exportDesc);
	}

	m_dxilLibDesc.DXILLibrary = CD3DX12_SHADER_BYTECODE{m_blob->GetBufferPointer(), m_blob->GetBufferSize()};
	m_dxilLibDesc.NumExports  = m_exports.size();
	m_dxilLibDesc.pExports	  = m_exports.data();

	return *this;
}

DxRayPipeline::HitGroup& DxRayPipeline::HitGroup::operator=(const HitGroup& rhs)
{
	// Move instead of copy to keep LPCWSTR pointers alive
	m_inputs	 = rhs.m_inputs;
	m_name		 = rhs.m_name;
	m_closestHit = rhs.m_closestHit;
	m_anyHit	 = rhs.m_anyHit;
	m_intersect	 = rhs.m_intersect;
	m_hitDesc	 = rhs.m_hitDesc;

	// Set up the LPCWSTRs again
	m_hitDesc.HitGroupExport		   = m_name.c_str();
	m_hitDesc.ClosestHitShaderImport   = m_closestHit.c_str();
	m_hitDesc.AnyHitShaderImport	   = (m_anyHit.empty()) ? nullptr : m_anyHit.c_str();
	m_hitDesc.IntersectionShaderImport = (m_intersect.empty()) ? nullptr : m_intersect.c_str();

	return *this;
}

DxRayPipeline::RootSignatureAssociation& DxRayPipeline::RootSignatureAssociation::operator=(const RootSignatureAssociation& rhs)
{
	m_rootSignature		   = rhs.m_rootSignature;
	m_rootSignaturePointer = rhs.m_rootSignaturePointer;
	m_symbols			   = rhs.m_symbols;

	m_symbolPointers.clear();
	for (const auto& symbol : m_symbols)
		m_symbolPointers.push_back(symbol.c_str());

	m_association.NumExports = m_symbolPointers.size();
	m_association.pExports	 = m_symbolPointers.data();
	return *this;
}
