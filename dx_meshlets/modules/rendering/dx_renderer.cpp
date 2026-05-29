module;

module dx_meshlets.rendering.dx_renderer;
import dx_wrapper.external.directx12;
import dx_wrapper.external.win32;

DxRenderer::DxRenderer(DxDevice* device) : m_device{device}
{
	m_renderRootSignature = DxRootSignature{};
	// In this order: Meshlets, Meshlet Vertices, Meshlet Triangles, Primitive Vertices
	m_renderRootSignature.AddDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 4);
	m_renderRootSignature.AddConstantBuffer(0); // Camera Const buffer
	m_renderRootSignature.Add32BitConstants(1, sizeof(ShaderMaterial));
	m_renderRootSignature.Add32BitConstants(2, sizeof(ShaderTransform));
	m_renderRootSignature.AddSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	m_renderRootSignature.Finalize(*m_device, "Meshlet Render Root Signature", true);

	m_renderPipeline = DxPipelineState{};
	m_renderPipeline.SetAmplificationShader("shaders/meshlet_amplification.hlsl")
			.SetMeshShader("shaders/meshlet_mesh.hlsl")
			.SetMeshPixelShader("shaders/meshlet_pixel.hlsl")
			.AddRenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
			.SetDepthRenderTarget(DXGI_FORMAT_D32_FLOAT)
			.SetCullMode(D3D12_CULL_MODE_NONE)
			.Finalize(*m_device, m_renderRootSignature, "Meshlet Pipeline");

	m_camera = Camera{m_device};
	m_camera.GetTransform().SetPosition(glm::vec3{0, 0, 4});

	m_cameraConstBuffer = DxConstBuffer<CameraConstBuffer>{*m_device, nullptr, ConstBufferType::Static};
}

void DxRenderer::AddModel(const std::filesystem::path& path) { m_models.emplace_back(*m_device, path); }

void DxRenderer::Render()
{
	// Update meshlet debug mode with input
	if (m_device->GetInput().GetKeyboardKeyOnce(Key::D1))
		m_meshletDebugMode = MeshletDebugMode::None;
	if (m_device->GetInput().GetKeyboardKeyOnce(Key::D2))
		m_meshletDebugMode = MeshletDebugMode::MeshletIndex;
	
	auto* commandList = m_device->GetDXDirectComList();

	commandList->SetGraphicsRootSignature(*m_renderRootSignature);
	commandList->SetPipelineState(*m_renderPipeline);

	const auto rtv = m_device->GetRTV();
	const auto dsv = m_device->GetDSV();

	static constexpr float rt_clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	commandList->ClearRenderTargetView(rtv, rt_clear_color, 0, nullptr);
	commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);

	commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

	m_camera.UpdateShaderCamera();
	CameraConstBuffer cameraCb = {m_camera.GetShaderCamera(), {glm::vec4{0.f}}};

	m_cameraConstBuffer.Bind(*m_device, std::move(cameraCb), MainRootParams::CameraCB);

	for (const auto& model : m_models)
	{
		for (const auto& node : model.GetNodes())
		{
			if (!node->m_mesh)
				continue;

			const auto& mesh = node->m_mesh;
			for (const auto& primitive : mesh->m_primitives)
			{
				primitive.Bind(*m_device, MainRootParams::PrimitiveSRVs);

				const auto& material = primitive.GetMaterial();
				if (material)
				{
					ShaderMaterial shaderMaterial;
					CompileShaderMaterial(*material, shaderMaterial);
					shaderMaterial.m_debugMode = std::to_underlying(m_meshletDebugMode);
					commandList->SetGraphicsRoot32BitConstants(MainRootParams::Material32C,
															   sizeof(ShaderMaterial) / sizeof(DWORD32),
															   &shaderMaterial,
															   0);
				}

				ShaderTransform shaderTransform;
				shaderTransform.m_worldMatrix = node->m_transform.GetWorldTransform();
				shaderTransform.m_normalMatrix =
						glm::mat4{glm::mat3{glm::transpose(glm::inverse(shaderTransform.m_worldMatrix))}};
				commandList->SetGraphicsRoot32BitConstants(MainRootParams::Transform32C,
														   sizeof(ShaderTransform) / sizeof(DWORD32),
														   &shaderTransform,
														   0);

				std::uint32_t threads = static_cast<std::uint32_t>((primitive.GetMeshletCount() / 32) + 1);
				commandList->DispatchMesh(threads, 1, 1);
			}
		}
	}
}

void DxRenderer::CompileShaderMaterial(const GltfMaterial& gltfMaterial, ShaderMaterial& shaderMaterial)
{
	shaderMaterial.m_alphaCutoff	   = gltfMaterial.m_alphaCutoff;
	shaderMaterial.m_emissiveFactor	   = gltfMaterial.m_emissiveFactor;
	shaderMaterial.m_baseColorFactor   = gltfMaterial.m_baseColorFactor;
	shaderMaterial.m_metallicFactor	   = gltfMaterial.m_metallicFactor;
	shaderMaterial.m_roughnessFactor   = gltfMaterial.m_roughnessFactor;
	shaderMaterial.m_normalScale	   = gltfMaterial.m_normalScale;
	shaderMaterial.m_occlusionStrength = gltfMaterial.m_occlusionStrength;

	const auto texIdx = gltfMaterial.GetTextureIndices();
	std::memcpy(&shaderMaterial.m_texIndices, texIdx.data(), 8 * sizeof(std::int32_t));

	shaderMaterial.m_flags = static_cast<std::uint32_t>(gltfMaterial.m_alphaMode);
	shaderMaterial.m_flags |= gltfMaterial.m_doubleSided ? (1 << 3) : 0;
}
