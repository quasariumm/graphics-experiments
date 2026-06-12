module;

export module dx_hw_ray.rendering.dx_render;
import std;
import dx_wrapper.core;
import dx_hw_ray.gltf.raytraced_model;
import dx_hw_ray.gltf.raytraced_primitive;
import dx_wrapper.rendering;
import dx_wrapper.resources.dx_const_buffer;
import dx_wrapper.external.glm;
import dx_hw_ray.helpers.tlas;
import dx_hw_ray.rendering.dx_ray_pipeline;
import dx_wrapper.resources.dx_render_texture;
import dx_wrapper.resources.dx_structured_buffer;
import dx_hw_ray.gltf.scene_geometry_buffer;
import dx_hw_ray.rendering.scene_lighting;
import dx_wrapper.resources.dx_texture;

export class DxRenderer
{
public:

	explicit DxRenderer(DxDevice* device);

	void AddModel(const std::filesystem::path& path);

	void Render();

	void Inspector();

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

	DxRootSignature m_rayGenRootSignature;
	DxRootSignature m_missRootSignature;
	DxRootSignature m_closestHitRootSignature;
	DxRayPipeline	m_renderPipeline;

	DxRootSignature m_blitRootSignature;
	DxPipelineState m_blitPipeline;

	DxTexture m_rayOutputTexture;
	DxTexture m_blitTexture;

	SceneGeometryBuffer m_sceneGeometryBuffer{};
	SceneLighting		m_sceneLighting;

	struct SceneConstBuffer
	{
		// Geometry buffers
		std::int32_t m_vertexBuffers;
		std::int32_t m_indexBuffers;
		std::int32_t m_materialsBuffers;
		std::int32_t m_materialIndicesBuffers;
		std::int32_t m_blasGeometryCounts;
		// Lighting
		std::int32_t  m_lightBuffer;
		std::uint32_t m_lightCount;
		// Frame parameters
		DebugMode	  m_debugMode;
		std::uint32_t m_maxRecursionDepth;
		std::uint32_t m_frameNum;
		std::uint32_t m_accumulationFame;
		/*
		 * Flags:
		 *  - accumulate (1 bit)
		 */
		std::uint32_t m_flags;
		glm::uvec4	  m_morePadding[13];
	};
	DxConstBuffer<SceneConstBuffer> m_sceneConstBuffer{};

	bool m_accumulate = false;

	std::vector<RaytracedModel> m_models;
	Tlas						m_tlas;
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
