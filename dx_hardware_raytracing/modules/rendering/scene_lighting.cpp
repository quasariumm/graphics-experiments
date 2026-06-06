module;

module dx_hw_ray.rendering.scene_lighting;
import dx_wrapper.helpers.dx_buffer_helpers;
import dx_wrapper.resources.dx_resource;
import dx_hw_ray.external.imgui;

SceneLighting::SceneLighting(const DxDevice& device)
{
	m_device = &device;
	CheckHR(CreateStaticBuffer(device,
							   nullptr,
							   sizeof(ShaderLight) * max_scene_lights,
							   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
							   m_gpuLightsBuffer,
							   "Scene Lights Buffer"));

	m_gpuLightsHeapIndex = device.GetShaderDescriptorPile().Allocate();
	CreateShaderResourceView(device,
							 device.GetShaderDescriptorPile(),
							 m_gpuLightsHeapIndex,
							 m_gpuLightsBuffer.Get(),
							 sizeof(ShaderLight));
}

SceneLighting::~SceneLighting()
{
	if (!m_device)
		return;
	m_device->GetShaderDescriptorPile().Free(m_gpuLightsHeapIndex);
}

void SceneLighting::AddDirectionalLight(const glm::vec3& direction, const glm::vec3& color, const float intensity)
{
	m_lights.push_back(ShaderLight{.m_type = 1, .m_direction = direction, .m_color = color, .m_intensity = intensity});
	Upload();
}

void SceneLighting::AddPointLight(const glm::vec3& position, const glm::vec3& color, float range, const float intensity)
{
	m_lights.push_back(ShaderLight{.m_position	 = position,
								   .m_type		 = 0,
								   .m_direction	 = glm::vec3{0.f, 1.f, 0.f},
								   .m_range		 = range,
								   .m_color		 = color,
								   .m_intensity	 = intensity,
								   .m_innerAngle = 0.f,
								   .m_outerAngle = std::numbers::pi_v<float>});
	Upload();
}

void SceneLighting::AddSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color,
								 const float range, const float intensity, const float innerAngle, const float outerAngle)
{
	m_lights.push_back(ShaderLight{.m_position	 = position,
								   .m_type		 = 0,
								   .m_direction	 = direction,
								   .m_range		 = range,
								   .m_color		 = color,
								   .m_intensity	 = intensity,
								   .m_innerAngle = innerAngle,
								   .m_outerAngle = outerAngle});
	Upload();
}

void SceneLighting::Inspector()
{
	bool changed = false;

	ImGui::SeparatorText("Lights");
	if (ImGui::Button("Add Directional Light"))
	{
		AddDirectionalLight(glm::vec3{0.f, -1.f, 0.f}, glm::vec3{0.f}, 1.f);
		changed = true;
	}

	if (ImGui::Button("Add Point Light"))
	{
		AddPointLight(glm::vec3{0.f}, glm::vec3{0.f}, 1.f, 1.f);
		changed = true;
	}

	if (ImGui::Button("Add Spot Light"))
	{
		AddSpotLight(glm::vec3{0.f}, glm::vec3{0.f, -1.f, 0.f}, glm::vec3{0.f}, 1.f, 1.f, 0.f, 0.5f);
		changed = true;
	}

	for (auto& light : m_lights)
	{
		ImGui::PushID(&light);
		if (light.m_type == 1)
		{
			ImGui::Text("Directional light");
			if (ImGui::DragFloat3("Direction", glm::value_ptr(light.m_direction), 0.001f))
			{
				changed			  = true;
				light.m_direction = glm::normalize(light.m_direction);
			}
			if (ImGui::ColorEdit3("Color", glm::value_ptr(light.m_color)))
				changed = true;
			if (ImGui::DragFloat("Intensity", &light.m_intensity, 0.01f))
				changed = true;
		}
		else
		{
			ImGui::Text("Punctual light");
			if (ImGui::DragFloat3("Position", glm::value_ptr(light.m_position), 0.01f))
				changed = true;
			if (ImGui::DragFloat3("Direction", glm::value_ptr(light.m_direction), 0.001f))
			{
				changed			  = true;
				light.m_direction = glm::normalize(light.m_direction);
			}
			if (ImGui::ColorEdit3("Color", glm::value_ptr(light.m_color)))
				changed = true;
			if (ImGui::DragFloat("Range", &light.m_range, 0.01f))
				changed = true;
			if (ImGui::DragFloat("Intensity", &light.m_intensity, 0.01f))
				changed = true;
			if (ImGui::DragFloat("Inner Angle", &light.m_innerAngle, 0.01f))
				changed = true;
			if (ImGui::DragFloat("Outer Angle", &light.m_outerAngle, 0.01f))
				changed = true;
		}
		ImGui::PopID();
		ImGui::Separator();
	}

	if (changed)
		Upload();
}

void SceneLighting::Upload() const
{
	m_device->GetResourceUpload().Begin();

	D3D12_SUBRESOURCE_DATA subData;
	subData.pData	   = m_lights.data();
	subData.RowPitch   = m_lights.size() * sizeof(ShaderLight);
	subData.SlicePitch = subData.RowPitch;

	Transition(m_device->GetDXDirectComList(),
			   m_gpuLightsBuffer.Get(),
			   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			   D3D12_RESOURCE_STATE_COPY_DEST);

	m_device->GetResourceUpload().Upload(m_gpuLightsBuffer.Get(), 0, &subData, 1);

	Transition(m_device->GetDXDirectComList(),
			   m_gpuLightsBuffer.Get(),
			   D3D12_RESOURCE_STATE_COPY_DEST,
			   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	m_device->GetResourceUpload().End(m_device->GetDXDirectComQueue());
}
