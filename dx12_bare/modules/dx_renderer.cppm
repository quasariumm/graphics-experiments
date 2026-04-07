module;

#include <filesystem>
#include <vector>

export module dx_bare.dx_render;
import dx_wrapper.core.dx_device;
import dx_wrapper.rendering.gltf;
import dx_wrapper.rendering.dx_pipelinestate;

export class DxRenderer
{
public:

	explicit DxRenderer(DxDevice* device);

	void AddModel(const std::filesystem::path& path);
	
	void Render();

private:

	DxDevice* m_device;
	
	DxRootSignature m_renderRootSignature;
	DxPipelineState m_renderPipeline;

	std::vector<GltfModel> m_models;
};
