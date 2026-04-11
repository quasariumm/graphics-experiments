module;

#include <fastgltf/types.hpp>

export module dx_wrapper.gltf.mesh;
import dx_wrapper.gltf.primitive;
import dx_wrapper.core.dx_device;

export struct GltfMesh
{
	explicit GltfMesh(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
					  const fastgltf::Mesh& mesh);

	std::vector<GltfPrimitive> m_primitives{};
};
