module;

module dx_hw_ray.rendering.dx_render;
import std;
import dx_hw_ray.external.dxr;
import dx_hw_ray.external.imgui;
import dx_wrapper.external.directx12;
import dx_wrapper.core.camera;
import dx_wrapper.external.glm;
import dx_wrapper.resources.dx_resource;
import dx_wrapper.resources.dx_texture;
import dx_wrapper.helpers.dx_buffer_helpers;

DxRenderer::DxRenderer(DxDevice* device)
	: m_rayOutputTexture{*device,
						 TextureType::D2,
						 DXGI_FORMAT_R8G8B8A8_UNORM,
						 static_cast<std::uint32_t>(device->GetWidth()),
						 static_cast<std::uint32_t>(device->GetHeight()),
						 1,
						 false,
						 true},
	  m_sceneLighting{*device}
{
	m_device = device;

	m_rayOutputTexture.CreateUAV(*m_device, 0);

	m_rayGenRootSignature = DxRootSignature{};
	m_rayGenRootSignature.AddDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1); // Output texture
	m_rayGenRootSignature.AddBufferSRV(0);											 // BVH
	m_rayGenRootSignature.AddConstantBuffer(0);										 // CameraCB
	m_rayGenRootSignature.Finalize(*m_device, "Ray Gen Root Signature", false, D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

	m_missRootSignature = DxRootSignature{};
	m_missRootSignature.Finalize(*m_device, "Miss Root Signature", false, D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

	m_closestHitRootSignature = DxRootSignature{};
	// Material buffer. Since that is not aligned, I create a descriptor just to be sure
	m_closestHitRootSignature.AddBufferSRV(0);		// BVH
	m_closestHitRootSignature.AddConstantBuffer(0); // SceneCB
	m_closestHitRootSignature.AddSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
										 D3D12_TEXTURE_ADDRESS_MODE_WRAP,
										 D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
										 D3D12_SHADER_VISIBILITY_ALL);
	m_closestHitRootSignature.Finalize(*m_device,
									   "Closest Hit Root Signature",
									   false,
									   D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

	m_camera = Camera{m_device};
	m_camera.GetTransform().SetPosition(glm::vec3{0, 0, 4});

	m_cameraConstBuffer = DxConstBuffer<CameraConstBuffer>{*m_device, nullptr, ConstBufferType::Static};
	m_sceneConstBuffer	= DxConstBuffer<SceneConstBuffer>{*m_device, nullptr, ConstBufferType::Static};

	m_renderPipeline = DxRayPipeline{};

	m_renderPipeline.AddRootSignatureAssociation(m_rayGenRootSignature, {"RayGen"})
			.AddRootSignatureAssociation(m_missRootSignature, {"Miss", "ShadowMiss"})
			.AddRootSignatureAssociation(m_closestHitRootSignature, {"ClosestHit"})
			.SetMaxPayloadSize(8 * sizeof(DWORD32))
			.SetMaxAttributesSize(2 * sizeof(DWORD32))
			.SetMaxRecursionDepth(8);
}

void DxRenderer::AddModel(const std::filesystem::path& path) { m_models.emplace_back(*m_device, path); }

void DxRenderer::Render()
{
	// Check resize
	if (m_device->GetWidth() != m_rayOutputTexture.GetWidth() || m_device->GetHeight() != m_rayOutputTexture.GetHeight())
	{
		m_rayOutputTexture.ResizeClear(*m_device, m_device->GetWidth(), m_device->GetHeight());
		m_camera.SetAspectRatio(*m_device);
	}

	// Generate a TLAS is not done yet
	static bool generatedTlas = false;
	if (!generatedTlas)
	{
		CreateRaySceneResources();

		const auto rayOutGpuHandle = m_device->GetShaderDescriptorPile().GetGpuHandleAt(m_rayOutputTexture.GetUavHeapIndex());

		m_renderPipeline.SetRayGenShader("shaders/ray_gen.hlsl",
										 "RayGen",
										 {reinterpret_cast<void*>(rayOutGpuHandle.ptr),
										  reinterpret_cast<void*>(m_tlas->GetGPUVirtualAddress()),
										  reinterpret_cast<void*>(m_cameraConstBuffer->GetGPUVirtualAddress())});

		m_renderPipeline.AddMissShader("shaders/miss.hlsl", "Miss");
		m_renderPipeline.AddMissShader("shaders/miss.hlsl", "ShadowMiss");

		m_renderPipeline.AddHitGroup("HitGroup",
									 "shaders/hit.hlsl",
									 "ClosestHit",
									 "",
									 "",
									 {reinterpret_cast<void*>(m_tlas->GetGPUVirtualAddress()),
									  reinterpret_cast<void*>(m_sceneConstBuffer->GetGPUVirtualAddress())});

		m_renderPipeline.Finalize(*m_device, "Main Ray Pipeline");

		generatedTlas = true;
	}

	auto* commandList = m_device->GetDXDirectComList();

	// Set up the camera const buffer
	m_camera.UpdateShaderCamera();
	CameraConstBuffer cameraCb{m_camera.GetShaderCamera()};
	// Rebind prevViewPos to inverseViewPos
	cameraCb.m_shaderCamera.m_prevViewProjectionMatrix = glm::inverse(cameraCb.m_shaderCamera.m_viewProjectionMatrix);
	m_cameraConstBuffer.UpdateData(*m_device, std::move(cameraCb));

	// Change scene data
	SceneConstBuffer& sceneConstBuffer	 = m_sceneConstBuffer.GetLocalStorage();
	sceneConstBuffer.m_lightCount		 = m_sceneLighting.GetLightCount();
	sceneConstBuffer.m_debugMode		 = m_debugMode;
	sceneConstBuffer.m_maxRecursionDepth = 8u;
	sceneConstBuffer.m_frameNum++;
	m_sceneConstBuffer.UpdateData(*m_device, sceneConstBuffer);

	/*
	 * Ray tracing
	 */
	m_rayOutputTexture.Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	D3D12_DISPATCH_RAYS_DESC desc{};

	auto sbtAddress = m_renderPipeline.GetShaderBindingTable()->GetGPUVirtualAddress();

	// RayGen
	const auto rayGenSize						= m_renderPipeline.GetRayGenSectionSize();
	desc.RayGenerationShaderRecord.SizeInBytes	= rayGenSize;
	desc.RayGenerationShaderRecord.StartAddress = sbtAddress;

	sbtAddress += rayGenSize;

	// Miss
	const auto missSize				   = m_renderPipeline.GetMissSectionSize();
	desc.MissShaderTable.SizeInBytes   = missSize;
	desc.MissShaderTable.StartAddress  = sbtAddress;
	desc.MissShaderTable.StrideInBytes = m_renderPipeline.GetMissEntrySize();

	sbtAddress += missSize;

	// HitGroup
	desc.HitGroupTable.SizeInBytes	 = m_renderPipeline.GetHitGroupSectionSize();
	desc.HitGroupTable.StartAddress	 = sbtAddress;
	desc.HitGroupTable.StrideInBytes = m_renderPipeline.GetHitGroupEntrySize();

	// Callable shaders
	desc.CallableShaderTable.SizeInBytes   = 0;
	desc.CallableShaderTable.StartAddress  = 0;
	desc.CallableShaderTable.StrideInBytes = 0;

	// Ray dispatch size
	desc.Width	= m_device->GetWidth();
	desc.Height = m_device->GetHeight();
	desc.Depth	= 1;

	// Bind the pipeline
	commandList->SetPipelineState1(*m_renderPipeline);
	commandList->DispatchRays(&desc);

	// Copy the output to the RTV
	auto* rt = m_device->GetRenderTarget();

	m_rayOutputTexture.Transition(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	Transition(commandList, rt, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);

	commandList->CopyResource(rt, *m_rayOutputTexture);

	Transition(commandList, rt, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void DxRenderer::Inspector()
{
	ImGui::SeparatorText("Performance");
	static float		   smoothFPS = 0.0f;
	static constexpr float fps_alpha = 0.05f; // lower = smoother, higher = more responsive

	const float dt		   = std::max(0.0001f, m_device->GetDeltaTime());
	const float currentFPS = 1.0f / dt;
	if (smoothFPS == 0.0f)
		smoothFPS = currentFPS;
	else
		smoothFPS += fps_alpha * (currentFPS - smoothFPS);

	ImGui::Text("%.1f fps", smoothFPS);
	ImGui::Text("%.1f ms", 1000.0f * dt);

	ImGui::SeparatorText("Debugging");

	// Debug mode selector
	const auto* const* debugModeOptions = debug_mode_names.data();
	auto*			   currentDebugMode = reinterpret_cast<int*>(&m_debugMode);
	ImGui::Combo("Debug Mode", currentDebugMode, debugModeOptions, debug_mode_names.size());

	m_sceneLighting.Inspector();
}

void DxRenderer::CreateRaySceneResources()
{
	m_tlas				  = Tlas{};
	m_sceneGeometryBuffer = SceneGeometryBuffer{};

	for (const auto& model : m_models)
	{
		m_sceneGeometryBuffer.AddModel(model);
		m_tlas.AddBlas(model.GetBlas(), glm::identity<glm::mat4>());
	}
	m_tlas.Generate(*m_device);
	m_sceneGeometryBuffer.Generate(*m_device);

	SceneConstBuffer sceneConstBuffer{};
	std::memset(&sceneConstBuffer, 0, sizeof(SceneConstBuffer));
	sceneConstBuffer.m_vertexBuffers		  = m_sceneGeometryBuffer.GetVertexHeapIndex();
	sceneConstBuffer.m_indexBuffers			  = m_sceneGeometryBuffer.GetIndexHeapIndex();
	sceneConstBuffer.m_materialsBuffers		  = m_sceneGeometryBuffer.GetMaterialsHeapIndex();
	sceneConstBuffer.m_materialIndicesBuffers = m_sceneGeometryBuffer.GetMaterialIndicesHeapIndex();
	sceneConstBuffer.m_blasGeometryCounts	  = m_sceneGeometryBuffer.GetBlasGeometryCountHeapIndex();
	sceneConstBuffer.m_lightBuffer			  = m_sceneLighting.GetGpuLightsHeapIndex();
	m_sceneConstBuffer.UpdateData(*m_device, std::move(sceneConstBuffer));
}
