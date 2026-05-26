module;

export module dx_wrapper.gltf.mesh;
import std;
import dx_wrapper.external.fastgltf;
import dx_wrapper.gltf.primitive;
import dx_wrapper.core.dx_device;

export template <typename Primitive = GltfPrimitive>
	requires std::is_base_of_v<GltfPrimitive, Primitive>
struct GltfMesh
{
	explicit GltfMesh(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
					  const fastgltf::Mesh& mesh);

	std::vector<Primitive> m_primitives{};
};

/*
 * Impl
 */

template <typename Primitive>
	requires std::is_base_of_v<GltfPrimitive, Primitive>
GltfMesh<Primitive>::GltfMesh(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
							  const fastgltf::Mesh& mesh)
{
	for (const auto& prim : mesh.primitives)
		m_primitives.emplace_back(device, modelPath, asset, prim);
}
