module;

export module dx_hw_ray.rendering.dx_render;
import std;
import dx_wrapper.core;
import dx_wrapper.gltf;
import dx_hw_ray.gltf.raytraced_primitive;
import dx_wrapper.rendering;
import dx_wrapper.resources.dx_const_buffer;
import dx_wrapper.external.glm;
import dx_hw_ray.helpers.tlas;
import dx_hw_ray.rendering.dx_ray_pipeline;
import dx_wrapper.resources.dx_render_texture;
import dx_wrapper.resources.dx_structured_buffer;
import dx_hw_ray.gltf.scene_geometry_buffer;

export class DxRenderer
{
public:

	explicit DxRenderer(DxDevice* device);

	void AddModel(const std::filesystem::path& path);

	void Render();

	Camera& GetCamera() { return m_camera; }

	/**
	 * @brief An enum over all possible debugging modes for the raytracer
	 * @attention [Note to devs] Because it's uint in the shader, DO NOT CHANGE THE UNDERLYING TYPE OF THIS
	 */
	enum class DebugMode : std::uint32_t
	{
		None = 0,
		Albedo,
		Alpha,
		Emissive,
		Roughness,
		Metallic,
		Transmissive,
		IOR,
		GeometryNormal,
		ShadedNormal,
		GeometryTangent,
		ShadedTangent,
		GeometryBitangent,
		ShadedBitangent,
		BRDF,
		PDF,
		Max // For use in loops. Do not use as an option
	};

	DebugMode m_debugMode = DebugMode::None;

private:

	void CreateRaySceneResources();

	DxDevice* m_device;
	Camera	  m_camera;

	struct CameraConstBuffer
	{
		Camera::ShaderCamera m_shaderCamera;
		glm::vec4			 m_padding[2];
	};
	static_assert(sizeof(CameraConstBuffer) % 256 == 0, "Camera const buffer is not 256-byte aligned");
	DxConstBuffer<CameraConstBuffer> m_cameraConstBuffer;

	struct ShaderMaterial
	{
		float m_alphaCutoff = 0.5f;

		glm::vec3 m_emissiveFactor{1.f};
		glm::vec4 m_baseColorFactor{1.f};

		float m_metallicFactor	  = 0.0f;
		float m_roughnessFactor	  = 1.0f;
		float m_normalScale		  = 1.0f;
		float m_occlusionStrength = 1.0f;

		int m_texIndices[8]{-1};

		std::uint32_t m_flags = 0;
		std::uint32_t m_padding[3];
	};
	static void CompileShaderMaterial(const GltfMaterial& gltfMaterial, ShaderMaterial& shaderMaterial);

	DxRootSignature m_rayGenRootSignature;
	DxRootSignature m_missRootSignature;
	DxRootSignature m_closestHitRootSignature;
	DxRayPipeline	m_renderPipeline;

	DxRenderTexture					   m_rayOutputTexture;
	DxStructuredBuffer<ShaderMaterial> m_materialBuffer;
	SceneGeometryBuffer				   m_sceneGeometryBuffer;

	struct SceneConstBuffer
	{
		std::int32_t  m_vertexBuffers;
		std::int32_t  m_indexBuffers;
		DebugMode	  m_debugMode;
		std::uint32_t m_maxRecursionDepth;
		glm::uvec4	  m_morePadding[15];
	};
	DxConstBuffer<SceneConstBuffer> m_sceneConstBuffer{};

	std::vector<GltfModel<RaytracedGltfPrimitive>> m_models;
	Tlas										   m_tlas;
};

export inline constexpr std::array<const char*, 16> debug_mode_names{
		"None",
		"Albedo",
		"Alpha",
		"Emissive",
		"Roughness",
		"Metallic",
		"Transmissive",
		"IOR",
		"Geometry Normal",
		"Shaded Normal",
		"Geometry Tangent",
		"Shaded Tangent",
		"Geometry Bitangent",
		"Shaded Bitangent",
		"BRDF",
		"PDF",
};
