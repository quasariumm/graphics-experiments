module;

export module dx_bare.rendering.dx_render;
import std;
import dx_wrapper.core;
import dx_wrapper.gltf;
import dx_wrapper.rendering;
import dx_wrapper.resources.dx_const_buffer;
import dx_wrapper.external.glm;

export class DxRenderer
{
public:

	explicit DxRenderer(DxDevice* device);

	void AddModel(const std::filesystem::path& path);

	void Render();

	Camera& GetCamera() { return m_camera; }

private:

	DxDevice* m_device;
	Camera	  m_camera;

	struct CameraConstBuffer
	{
		Camera::ShaderCamera m_shaderCamera;
		glm::vec4			 m_padding[2];
	};
	static_assert(sizeof(CameraConstBuffer) % 256 == 0, "Camera const buffer is not 256-byte aligned");
	DxConstBuffer<CameraConstBuffer> m_cameraConstBuffer;
	
	struct ShaderTransform
	{
		glm::mat4 m_worldMatrix;
		glm::mat4 m_normalMatrix;
	};
	
	struct ShaderMaterial 
	{
		float m_alphaCutoff;

		glm::vec3 m_emissiveFactor;
		glm::vec4 m_baseColorFactor;

		float m_metallicFactor;
		float m_roughnessFactor;
		float m_normalScale;
		float m_occlusionStrength;

		int m_texIndices[8];
	
		std::uint32_t m_flags;
		std::uint32_t m_padding[3];
	};
	static void CompileShaderMaterial(const GltfMaterial& gltfMaterial, ShaderMaterial& shaderMaterial);

	DxRootSignature m_renderRootSignature;
	DxPipelineState m_renderPipeline;

	std::vector<GltfModel<>> m_models;
};
