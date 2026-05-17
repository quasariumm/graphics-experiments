module;

module dx_meshlets.gltf.mesh;
import std;
import dx_wrapper.external.fastgltf;

GltfMesh::GltfMesh(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset, const fastgltf::Mesh& mesh)
{
	for (const auto& prim : mesh.primitives)
		m_primitives.emplace_back(device, modelPath, asset, prim);
}