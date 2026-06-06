module;

module dx_hw_ray.gltf.raytraced_model;
import dx_wrapper.core.dx_common;
import dx_wrapper.gltf.material;
import dx_wrapper.helpers.dx_buffer_helpers;

inline ShaderMaterial CompileShaderMaterial(const GltfMaterial& gltfMaterial)
{
	ShaderMaterial shaderMaterial;
	shaderMaterial.m_alphaCutoff	   = gltfMaterial.m_alphaCutoff;
	shaderMaterial.m_emissiveFactor	   = gltfMaterial.m_emissiveFactor;
	shaderMaterial.m_baseColorFactor   = gltfMaterial.m_baseColorFactor;
	shaderMaterial.m_metallicFactor	   = gltfMaterial.m_metallicFactor;
	shaderMaterial.m_roughnessFactor   = gltfMaterial.m_roughnessFactor;
	shaderMaterial.m_normalScale	   = gltfMaterial.m_normalScale;
	shaderMaterial.m_occlusionStrength = gltfMaterial.m_occlusionStrength;

	const auto texIdx = gltfMaterial.GetTextureIndices();
	std::memcpy(&shaderMaterial.m_texIndices, texIdx.data(), 8 * sizeof(std::int32_t));

	shaderMaterial.m_flags = static_cast<std::uint32_t>(gltfMaterial.m_alphaMode);
	shaderMaterial.m_flags |= gltfMaterial.m_doubleSided ? (1 << 3) : 0;
	shaderMaterial.m_flags |= gltfMaterial.GetTextureCoordinateMap() << 16;
	return shaderMaterial;
}

RaytracedModel::RaytracedModel(DxDevice& device, const std::filesystem::path& modelPath)
	: GltfModel{device, modelPath}
{
	// Create the material list
	std::vector<ShaderMaterial> shaderMaterials;
	for (const auto& material : GetMaterials())
		shaderMaterials.push_back(CompileShaderMaterial(*material));

	m_materialsList = DxStructuredBuffer{device, shaderMaterials, "Raytraced Model Materials List"};

	// Create the material index list
	std::vector<std::int32_t> materialIndices;
	// Add geometry instances to the BLAS based on the nodes
	for (const auto& node : GetNodes())
	{
		if (!node || !node->m_mesh)
			continue;

		for (auto& primitive : node->m_mesh->m_primitives)
		{
			// Add instance to the BLAS
			primitive.AddInstance(device, m_blas, node->m_transform.GetWorldTransform());
			// Add index into material list
			materialIndices.push_back(primitive.m_materialIndex);
		}
	}

	CheckHR(CreateStaticBuffer(device,
							   materialIndices,
							   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
							   m_materialIndicesList,
							   "Raytraced Model Material Indices List"));

	// Build the BLAS
	m_blas.Generate(device);
}

const Blas&		RaytracedModel::GetBlas() const { return m_blas; }
ID3D12Resource* RaytracedModel::GetMaterialList() const { return m_materialsList.GetResource(); }
ID3D12Resource* RaytracedModel::GetMaterialIndicesList() const { return m_materialIndicesList.Get(); }
