module;

export module dx_wrapper.gltf.model;
import std;
import dx_wrapper.core.common;
import dx_wrapper.core.dx_device;
import dx_wrapper.external.glm;
import dx_wrapper.external.fastgltf;
import dx_wrapper.gltf.node;
import dx_wrapper.gltf.mesh;
import dx_wrapper.gltf.primitive;
import dx_wrapper.gltf.material;

export template <typename T = GltfPrimitive>
	requires std::is_base_of_v<GltfPrimitive, T>
class GltfModel
{
public:

	explicit GltfModel(DxDevice& device, const std::filesystem::path& path);

	const std::vector<std::shared_ptr<GltfMesh<T>>>&  GetMeshes() const { return m_meshes; }
	const std::vector<std::shared_ptr<GltfNode<T>>>&  GetNodes() const { return m_nodes; }
	const std::vector<std::shared_ptr<GltfMaterial>>& GetMaterials() const { return m_materials; }

private:

	std::vector<std::shared_ptr<GltfMesh<T>>>  m_meshes{};
	std::vector<std::shared_ptr<GltfNode<T>>>  m_nodes{};
	std::vector<std::shared_ptr<GltfMaterial>> m_materials{};
};

/*
 * Impl
 */

inline glm::vec3 ToGlm(fastgltf::math::nvec3 v) { return {v.x(), v.y(), v.z()}; }
inline glm::quat ToGlm(fastgltf::math::fquat q) { return {q.w(), q.x(), q.y(), q.z()}; }

template <typename T>
	requires std::is_base_of_v<GltfPrimitive, T>
GltfModel<T>::GltfModel(DxDevice& device, const std::filesystem::path& path)
{
	m_materials.clear();
	m_meshes.clear();
	m_nodes.clear();

	fastgltf::Parser parser{};

	constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::LoadExternalBuffers |
								 fastgltf::Options::GenerateMeshIndices | fastgltf::Options::DecomposeNodeMatrices;

	auto gltfFile = fastgltf::MappedGltfFile::FromPath(path);
	auto asset	  = *parser.loadGltf(gltfFile.get(), path.parent_path(), gltfOptions);

	const auto& scene = asset.scenes[0];

	// Parse materials
	m_materials.clear();
	m_materials.reserve(asset.materials.size());
	for (const auto& material : asset.materials)
	{
		m_materials.emplace_back(std::make_shared<GltfMaterial>(device, path, asset, material));
	}

	// Parse meshes
	m_meshes.clear();
	m_meshes.reserve(asset.meshes.size());

	for (const auto& mesh : asset.meshes)
		m_meshes.push_back(std::make_shared<GltfMesh<T>>(device, path, asset, mesh, m_materials));

	// Parse scene nodes
	auto function = [&](const size_t nodeIndex, std::shared_ptr<GltfNode<T>> parent, auto& self) -> void
	{
		if (asset.nodes.size() <= nodeIndex)
			throw std::logic_error("Node index goes outside the nodes array");
		auto& gltfNode = asset.nodes[nodeIndex];

		auto node = std::make_shared<GltfNode<T>>();
		m_nodes.push_back(node);
		node->m_parent = parent;
		node->m_transform.SetParent(&parent->m_transform);

		// Safe because of fastgltf::Options::DecomposeNodeMatrices
		auto [translation, rotation, scale] = std::get<fastgltf::TRS>(gltfNode.transform);
		node->m_transform.SetScale(ToGlm(scale));
		node->m_transform.SetRotation(ToGlm(rotation));
		node->m_transform.SetPosition(ToGlm(translation));

		if (gltfNode.meshIndex)
			node->m_mesh = m_meshes[*gltfNode.meshIndex];

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
