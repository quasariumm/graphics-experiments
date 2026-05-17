module;

export module dx_meshlets.gltf.mesh;
import std;
import dx_wrapper.external.fastgltf;
import dx_meshlets.gltf.primitive;
import dx_wrapper.core.dx_device;

export struct GltfMesh
{
	explicit GltfMesh(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
					  const fastgltf::Mesh& mesh);

	std::vector<GltfPrimitive> m_primitives{};
};
