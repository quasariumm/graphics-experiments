module;

#include <filesystem>
#include <vector>

export module dx_bare.rendering.dx_render;
import dx_wrapper.core;
import dx_wrapper.rendering;
import dx_wrapper.resources.dx_const_buffer;
import dx_wrapper.external.glm;
import dx_bare.core.camera;

export class DxRenderer
{
public:

	explicit DxRenderer(DxDevice* device);

	void AddModel(const std::filesystem::path& path);
	
	void Render();

private:

	DxDevice* m_device;
	Camera m_camera;
	
	struct CameraConstBuffer
	{
		Camera::ShaderCamera m_shaderCamera;
		glm::vec4 m_padding[2];
	};
	static_assert(sizeof(CameraConstBuffer) % 256 == 0, "Camera const buffer is not 256-byte aligned");
	DxConstBuffer<CameraConstBuffer> m_cameraConstBuffer;
	
	DxRootSignature m_renderRootSignature;
	DxPipelineState m_renderPipeline;

	std::vector<GltfModel> m_models;
};
