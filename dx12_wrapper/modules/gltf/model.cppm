module;

#include <filesystem>
#include <memory>

export module dx_wrapper.gltf.model;
import dx_wrapper.core.dx_device;
import dx_wrapper.gltf.node;
import dx_wrapper.gltf.mesh;

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
