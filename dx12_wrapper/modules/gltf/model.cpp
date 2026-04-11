module;

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>

module dx_wrapper.gltf.model;
import dx_wrapper.core.dx_device;
import dx_wrapper.core.log;

GltfModel::GltfModel(DxDevice& device, const std::filesystem::path& path)
{
	fastgltf::Parser parser{};

	constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble |
								 fastgltf::Options::LoadExternalBuffers | fastgltf::Options::GenerateMeshIndices;
	
	auto gltfFile = fastgltf::MappedGltfFile::FromPath(path);
	auto asset	  = *parser.loadGltf(gltfFile.get(), path.parent_path(), gltfOptions);

	fastgltf::iterateSceneNodes(asset,
								0,
								fastgltf::math::fmat4x4(),
								[&](fastgltf::Node& node, fastgltf::math::fmat4x4)
								{
									if (node.meshIndex.has_value())
									{
										m_meshes.emplace_back(device, path, asset, asset.meshes[*node.meshIndex]);
									}
								});
}