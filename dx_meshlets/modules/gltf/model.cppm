module;

export module dx_meshlets.gltf.model;
import std;
import dx_wrapper.core.dx_device;
import dx_meshlets.gltf.node;
import dx_meshlets.gltf.mesh;

export class GltfModel
{
public:

	explicit GltfModel(DxDevice& device, const std::filesystem::path& path);

	const std::vector<std::shared_ptr<GltfMesh>>& GetMeshes() const { return m_meshes; }
	const std::vector<std::shared_ptr<GltfNode>>& GetNodes() const { return m_nodes; }

private:

	std::vector<std::shared_ptr<GltfMesh>> m_meshes{};
	std::vector<std::shared_ptr<GltfNode>> m_nodes{};
};
