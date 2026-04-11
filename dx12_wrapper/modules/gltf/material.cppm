module;

#include <fastgltf/types.hpp>

export module dx_wrapper.gltf.material;
import dx_wrapper.core.dx_device;
import dx_wrapper.external.glm;
import dx_wrapper.resources.dx_texture;

export class GltfMaterial
{
public:

	using AlphaMode = fastgltf::AlphaMode;

	GltfMaterial() = default;
	explicit GltfMaterial(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
						  const fastgltf::Material& material);

	std::string m_name = "Unnamed Material";

	/*
	 * Properties
	 */

	float	  m_alphaCutoff = 0.5f;
	AlphaMode m_alphaMode	= AlphaMode::Opaque;

	bool m_doubleSided = false;

	glm::vec3 m_emissiveFactor{1.0f};
	glm::vec4 m_baseColorFactor{1.0f};
	float	  m_metallicFactor	  = 1.0f;
	float	  m_roughnessFactor	  = 1.0f;
	float	  m_normalScale		  = 1.0f;
	float	  m_occlusionStrength = 1.0f;

	/*
	 * Textures
	 */

	auto GetBaseColorTexture() { return m_baseColorTexture; }
	auto GetNormalTexture() { return m_normalTexture; }
	auto GetEmissiveTexture() { return m_emissiveTexture; }
	auto GetOcclusionTexture() { return m_occlusionTexture; }
	auto GetRoughMetalTexture() { return m_roughMetalTexture; }
	
	std::array<int, 8> GetTextureIndices() const;

private:

	using MaterialTexture = std::optional<std::shared_ptr<DxTexture>>;

	/** Albedo (RGBA) */
	MaterialTexture m_baseColorTexture;
	/** Normals (RGB) */
	MaterialTexture m_normalTexture;
	/** Emissive (RGB) */
	MaterialTexture m_emissiveTexture;
	/** Occlusion (R) */
	MaterialTexture m_occlusionTexture;
	/** Roughness (G) + Metallic (B) */
	MaterialTexture m_roughMetalTexture;

	static void ProcessTexture(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
						const fastgltf::TextureInfo& textureInfo, MaterialTexture& result);
};
