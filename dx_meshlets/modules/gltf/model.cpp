module;

#include <cassert>

module dx_meshlets.gltf.model;
import std;
import dx_wrapper.external.glm;
import dx_wrapper.external.fastgltf;
import dx_wrapper.core.dx_device;
import dx_wrapper.core.log;

inline glm::vec3 ToGlm(fastgltf::math::nvec3 v) { return {v.x(), v.y(), v.z()}; }
inline glm::quat ToGlm(fastgltf::math::fquat q) { return {q.w(), q.x(), q.y(), q.z()}; }

GltfModel::GltfModel(DxDevice& device, const std::filesystem::path& path)
{
	fastgltf::Parser parser{};

	constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::LoadExternalBuffers |
								 fastgltf::Options::GenerateMeshIndices | fastgltf::Options::DecomposeNodeMatrices;

	auto gltfFile = fastgltf::MappedGltfFile::FromPath(path);
	auto asset	  = *parser.loadGltf(gltfFile.get(), path.parent_path(), gltfOptions);

	auto& scene = asset.scenes[0];

	m_meshes.resize(asset.meshes.size());

	auto function = [&](const size_t nodeIndex, std::shared_ptr<GltfNode> parent, auto& self) -> void
	{
		assert(asset.nodes.size() > nodeIndex);
		auto& gltfNode = asset.nodes[nodeIndex];

		auto node	   = std::make_shared<GltfNode>();
		m_nodes.push_back(node);
		node->m_parent = parent;
		node->m_transform.SetParent(&parent->m_transform);
		
		// Safe because of fastgltf::Options::DecomposeNodeMatrices
		auto [translation, rotation, scale] = std::get<fastgltf::TRS>(gltfNode.transform);
		node->m_transform.SetScale(ToGlm(scale));
		node->m_transform.SetRotation(ToGlm(rotation));
		node->m_transform.SetPosition(ToGlm(translation));

		if (gltfNode.meshIndex)
		{
			if (!m_meshes[*gltfNode.meshIndex])
				m_meshes[*gltfNode.meshIndex] =
						std::make_shared<GltfMesh>(device, path, asset, asset.meshes[*gltfNode.meshIndex]);
			
			node->m_mesh = m_meshes[*gltfNode.meshIndex];
		}

		for (auto& child : gltfNode.children)
		{
			self(child, node, self);
		}
	};

	for (const auto& sceneNode : scene.nodeIndices)
	{
		function(sceneNode, nullptr, function);
	}
}
