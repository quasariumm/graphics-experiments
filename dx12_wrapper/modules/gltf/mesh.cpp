module;

#include <filesystem>
#include <fastgltf/core.hpp>

module dx_wrapper.gltf.mesh;

GltfMesh::GltfMesh(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset, const fastgltf::Mesh& mesh)
{
	for (const auto& prim : mesh.primitives)
		m_primitives.emplace_back(device, modelPath, asset, prim);
}