module;

export module dx_hw_ray.gltf.raytraced_model;
import std;
import dx_wrapper.external.glm;
import dx_wrapper.gltf.model;
import dx_wrapper.resources.dx_structured_buffer;
import dx_hw_ray.gltf.raytraced_primitive;
import dx_hw_ray.helpers.blas;

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

export class RaytracedModel : public GltfModel<RaytracedGltfPrimitive>
{

public:

	explicit RaytracedModel(DxDevice& device, const std::filesystem::path& modelPath);
	
	const Blas& GetBlas() const;
	
	ID3D12Resource* GetMaterialList() const;
	ID3D12Resource* GetMaterialIndicesList() const;

private:

	/** Stores the materials compiled from the model's material list  */
	DxStructuredBuffer<ShaderMaterial> m_materialsList;
	/** Stores indices per primitive into the material list */
	ComPtr<ID3D12Resource> m_materialIndicesList;

	Blas m_blas;
};
