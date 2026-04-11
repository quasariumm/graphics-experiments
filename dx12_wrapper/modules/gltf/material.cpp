module;

#include <fastgltf/types.hpp>

module dx_wrapper.gltf.material;
import dx_wrapper.resources.resource_bank;

glm::vec2 ToGlm(fastgltf::math::nvec2 v) { return {v.x(), v.y()}; }
glm::vec3 ToGlm(fastgltf::math::nvec3 v) { return {v.x(), v.y(), v.z()}; }
glm::vec4 ToGlm(fastgltf::math::nvec4 v) { return {v.x(), v.y(), v.z(), v.w()}; }

GltfMaterial::GltfMaterial(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
						   const fastgltf::Material& material)
{
	if (!material.name.empty())
		m_name = material.name;
	m_alphaCutoff = material.alphaCutoff;
	m_alphaMode	  = material.alphaMode;
	m_doubleSided = material.doubleSided;

	m_emissiveFactor  = ToGlm(material.emissiveFactor) * material.emissiveStrength;
	m_baseColorFactor = ToGlm(material.pbrData.baseColorFactor);
	m_roughnessFactor = material.pbrData.roughnessFactor;
	m_metallicFactor  = material.pbrData.metallicFactor;

	if (material.pbrData.baseColorTexture)
		ProcessTexture(device, modelPath, asset, *material.pbrData.baseColorTexture, m_baseColorTexture);

	if (material.emissiveTexture)
		ProcessTexture(device, modelPath, asset, *material.emissiveTexture, m_emissiveTexture);

	if (material.normalTexture)
	{
		m_normalScale = material.normalTexture->scale;
		ProcessTexture(device, modelPath, asset, *material.normalTexture, m_normalTexture);
	}

	if (material.occlusionTexture)
	{
		m_occlusionStrength = material.occlusionTexture->strength;
		ProcessTexture(device, modelPath, asset, *material.occlusionTexture, m_occlusionTexture);
	}

	if (material.pbrData.metallicRoughnessTexture)
		ProcessTexture(device, modelPath, asset, *material.pbrData.metallicRoughnessTexture, m_roughMetalTexture);
}

std::array<int, 8> GltfMaterial::GetTextureIndices() const
{
	std::array res{-1, -1, -1, -1, -1, -1, -1, -1};
	if (m_baseColorTexture)
		res[0] = m_baseColorTexture.value()->GetSrvHeapIndex();
	if (m_normalTexture)
		res[1] = m_normalTexture.value()->GetSrvHeapIndex();
	if (m_emissiveTexture)
		res[2] = m_emissiveTexture.value()->GetSrvHeapIndex();
	if (m_occlusionTexture)
		res[3] = m_occlusionTexture.value()->GetSrvHeapIndex();
	if (m_roughMetalTexture)
		res[4] = m_roughMetalTexture.value()->GetSrvHeapIndex();
	return res;
}

template <class... Ts>
struct Overloads : Ts...
{
	using Ts::operator()...;
};

void GltfMaterial::ProcessTexture(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
								  const fastgltf::TextureInfo& textureInfo, MaterialTexture& result)
{
	const fastgltf::Texture&	texture = asset.textures[textureInfo.textureIndex];
	const fastgltf::DataSource& data	= asset.images[*texture.imageIndex].data;

	auto visitor =
			Overloads{[&](const fastgltf::sources::URI& uri)
					  {
						  auto location = modelPath.parent_path() / uri.uri.path();
						  result = device.GetResourceBank().GetOrCreateResource<DxTexture>(location.string(), device, location);
					  },
					  [&](const fastgltf::sources::Array& array)
					  {
						  const auto identifier = std::format("{}:embedded{}", modelPath.string(), *texture.imageIndex);
						  result				= device.GetResourceBank().GetOrCreateResource<DxTexture>(identifier,
																							  device,
																							  array.bytes.data(),
																							  array.bytes.size_bytes());
					  },
					  [&](const fastgltf::sources::Vector& vector)
					  {
						  const auto identifier = std::format("{}:embedded{}", modelPath.string(), *texture.imageIndex);
						  result				= device.GetResourceBank().GetOrCreateResource<DxTexture>(identifier,
																							  device,
																							  vector.bytes.data(),
																							  vector.bytes.size());
					  },
					  [](const auto&) {}};

	std::visit<void>(visitor, data);
}
