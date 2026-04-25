module;

#include <d3d12.h>
#include <filesystem>

module dx_bare.rendering.dx_render;
import dx_bare.core.camera;
import dx_wrapper.external.glm;

DxRenderer::DxRenderer(DxDevice* device)
{
	m_device = device;

	m_renderRootSignature = DxRootSignature{};
	m_renderRootSignature.AddConstantBuffer(0);
	m_renderRootSignature.Add32BitConstants(1, sizeof(ShaderMaterial));
	m_renderRootSignature.Add32BitConstants(2, sizeof(ShaderTransform));
	m_renderRootSignature.AddSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	m_renderRootSignature.Finalize(*m_device, "Main Render Root Signature", true);

	m_renderPipeline = DxPipelineState{};
	m_renderPipeline.SetVertexShader("../shaders/main_vertex.hlsl")
			.SetPixelShader("../shaders/main_pixel.hlsl")
			.AddRenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
			.SetDepthRenderTarget(DXGI_FORMAT_D32_FLOAT)
			.SetCullMode(D3D12_CULL_MODE_NONE)
			.AddVertexInput("POSITION", DXGI_FORMAT_R32G32B32_FLOAT)
			.AddVertexInput("UV_PRIM", DXGI_FORMAT_R32G32_FLOAT)
			.AddVertexInput("UV_SEC", DXGI_FORMAT_R32G32_FLOAT)
			.AddVertexInput("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT)
			.AddVertexInput("TANGENT", DXGI_FORMAT_R32G32B32A32_FLOAT)
			.Finalize(*m_device, m_renderRootSignature, "Main Render Pipeline", "dx12_wrapper");

	m_camera = Camera{m_device};
	m_camera.GetTransform().SetPosition(glm::vec3{0, 0, 4});

	m_cameraConstBuffer = DxConstBuffer<CameraConstBuffer>{*m_device, nullptr, ConstBufferType::Static};
}

void DxRenderer::AddModel(const std::filesystem::path& path) { m_models.emplace_back(*m_device, path); }

void DxRenderer::Render()
{
	auto* commandList = m_device->GetDXDirectComList();

	commandList->SetGraphicsRootSignature(*m_renderRootSignature);
	commandList->SetPipelineState(*m_renderPipeline);

	const auto rtv = m_device->GetRTV();
	const auto dsv = m_device->GetDSV();

	static constexpr float rt_clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	commandList->ClearRenderTargetView(rtv, rt_clear_color, 0, nullptr);
	commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);

	commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_camera.UpdateShaderCamera();
	CameraConstBuffer cameraCb = {m_camera.GetShaderCamera(), {glm::vec4{0.f}}};

	m_cameraConstBuffer.Bind(*m_device, &cameraCb, 0);

	for (const auto& model : m_models)
	{
		for (const auto& node : model.GetNodes())
		{
			if (!node->m_mesh)
				continue;

			const auto& mesh = node->m_mesh;
			for (const auto& primitive : mesh->m_primitives)
			{
				primitive.Bind(*m_device, 0);

				const auto& material = primitive.GetMaterial();
				if (material)
				{
					ShaderMaterial shaderMaterial;
					CompileShaderMaterial(*material, shaderMaterial);
					commandList->SetGraphicsRoot32BitConstants(1, sizeof(ShaderMaterial) / sizeof(DWORD32), &shaderMaterial, 0);
				}
				
				ShaderTransform shaderTransform;
				shaderTransform.m_worldMatrix = node->m_transform.GetWorldTransform();
				shaderTransform.m_normalMatrix = glm::mat4{glm::mat3{glm::transpose(glm::inverse(shaderTransform.m_worldMatrix))}};
				commandList->SetGraphicsRoot32BitConstants(2, sizeof(ShaderTransform) / sizeof(DWORD32), &shaderTransform, 0);

				commandList->DrawIndexedInstanced(primitive.GetIndices().size(), 1, 0, 0, 0);
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
	memcpy(&shaderMaterial.m_texIndices, texIdx.data(), 8 * sizeof(int32_t));

	shaderMaterial.m_flags = static_cast<uint32_t>(gltfMaterial.m_alphaMode);
	shaderMaterial.m_flags |= gltfMaterial.m_doubleSided ? (1 << 3) : 0;
}
