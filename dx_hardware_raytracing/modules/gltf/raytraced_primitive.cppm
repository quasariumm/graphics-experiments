module;

export module dx_hw_ray.gltf.raytraced_primitive;
import std;
import dx_wrapper.external.fastgltf;
import dx_wrapper.gltf.primitive;
import dx_hw_ray.helpers.blas;

export class RaytracedGltfPrimitive final : public GltfPrimitive
{

public:

	explicit RaytracedGltfPrimitive(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
									const fastgltf::Primitive& primitive);

	~RaytracedGltfPrimitive() override = default;
	
	const Blas& GetBlas() const { return m_blas; }

private:

	Blas m_blas;
};
