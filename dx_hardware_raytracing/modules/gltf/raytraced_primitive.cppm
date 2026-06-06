module;

export module dx_hw_ray.gltf.raytraced_primitive;
import std;
import dx_wrapper.external.glm;
import dx_wrapper.external.fastgltf;
import dx_wrapper.gltf.common;
import dx_wrapper.gltf.primitive;
import dx_hw_ray.helpers.blas;
import dx_wrapper.gltf.material;

export class RaytracedGltfPrimitive final : public GltfPrimitive
{

public:

	explicit RaytracedGltfPrimitive(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
									const fastgltf::Primitive& primitive, const MaterialsList& materials);

	~RaytracedGltfPrimitive() override = default;

	void AddInstance(const DxDevice& device, Blas& blas, const glm::mat4& transform) const;

private:

	friend class RaytracedModel;

	std::int32_t m_materialIndex = -1;
};
