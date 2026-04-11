module;

#include <filesystem>

export module dx_wrapper.gltf.model;
import dx_wrapper.gltf.mesh;

export class GltfModel
{
public:

	explicit GltfModel(DxDevice& device, const std::filesystem::path& path);

	const std::vector<GltfMesh>& GetMeshes() const { return m_meshes; }

private:

	std::vector<GltfMesh> m_meshes{};
};
