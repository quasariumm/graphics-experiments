module;

export module dx_meshlets.rendering.dx_renderer;
import std;
import dx_meshlets.gltf;
import dx_wrapper.external.glm;
import dx_wrapper.core.dx_device;
import dx_meshlets.core.camera;
import dx_wrapper.rendering.dx_pipelinestate;
import dx_wrapper.resources.dx_const_buffer;

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
	
	struct MainRootParams
	{
		// NOLINTBEGIN
		// Because this is an "enum"
		static constexpr std::uint32_t PrimitiveSRVs = 0;
		static constexpr std::uint32_t CameraCB = 1;
		static constexpr std::uint32_t Material32C = 2;
		static constexpr std::uint32_t Transform32C = 3;
		// NOLINTENT
	};

	std::vector<GltfModel> m_models;
};