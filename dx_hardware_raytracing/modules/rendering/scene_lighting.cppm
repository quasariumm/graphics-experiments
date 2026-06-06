module;

export module dx_hw_ray.rendering.scene_lighting;
import std;
import dx_wrapper.core.dx_common;
import dx_wrapper.core.dx_device;
import dx_wrapper.external.glm;
import dx_wrapper.rendering.dx_descriptor_pile;

export class SceneLighting
{
public:

	explicit SceneLighting(const DxDevice& device);
	~SceneLighting();

	void AddDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);

	void AddPointLight(const glm::vec3& position, const glm::vec3& color, float range, float intensity);

	void AddSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float range,
					  float intensity, float innerAngle, float outerAngle);

	void Inspector();

	[[nodiscard]] DxDescriptorPile::IndexType GetGpuLightsHeapIndex() const { return m_gpuLightsHeapIndex; }
	[[nodiscard]] std::size_t GetLightCount() const { return m_lights.size(); }

private:

	void Upload() const;

	const DxDevice* m_device;

	struct ShaderLight
	{
		glm::vec3  m_position{0.f};
		int		   m_type{0};
		glm::vec3  m_direction{0.f};
		float	   m_range{0.f};
		glm::vec3  m_color{0.f};
		float	   m_intensity{0.f};
		float	   m_innerAngle{0.f};
		float	   m_outerAngle{0.f};
		glm::uvec2 m_padding{0};
	};

	std::vector<ShaderLight> m_lights;

	static constexpr std::size_t max_scene_lights = 128;
	ComPtr<ID3D12Resource>		 m_gpuLightsBuffer;
	DxDescriptorPile::IndexType	 m_gpuLightsHeapIndex = DxDescriptorPile::invalid;
};
