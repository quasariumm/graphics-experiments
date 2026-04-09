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
	m_renderRootSignature.Finalize(*m_device, "Main Render Root Signature");

	m_renderPipeline = DxPipelineState{};
	m_renderPipeline.SetVertexShader("../shaders/main_vertex.hlsl")
			.SetPixelShader("../shaders/main_pixel.hlsl")
			.AddRenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
			.SetDepthRenderTarget(DXGI_FORMAT_D32_FLOAT)
			.SetCullMode(D3D12_CULL_MODE_NONE)
			.AddVertexInput("POSITION", DXGI_FORMAT_R32G32B32_FLOAT)
			.Finalize(*m_device, m_renderRootSignature, "Main Render Pipeline", "dx12_wrapper");
	
	m_camera = Camera{m_device};
	m_camera.GetTransform().SetPosition(glm::vec3{0, 0, 4});
	
	m_cameraConstBuffer.Init(*m_device, nullptr, ConstBufferType::Static);
}

void DxRenderer::AddModel(const std::filesystem::path& path) { m_models.emplace_back(*m_device, path); }

void DxRenderer::Render()
{
	auto* commandList = m_device->GetDXDirectComList();

	commandList->SetGraphicsRootSignature(*m_renderRootSignature);
	commandList->SetPipelineState(*m_renderPipeline);
	
	const auto rtv = m_device->GetRTV();
	const auto dsv = m_device->GetDSV();
	
	static constexpr float rt_clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView(rtv, rt_clear_color, 0, nullptr);
	commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
	
	commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
	
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	m_camera.UpdateShaderCamera();
	CameraConstBuffer cameraCb = {
		m_camera.GetShaderCamera(),
		{glm::vec4{0.f}}
	};
	
	m_cameraConstBuffer.Bind(*m_device, &cameraCb, 0);

	for (const auto& model : m_models)
	{
		for (const auto& mesh : model.GetMeshes())
		{
			for (const auto& primitive : mesh.m_primitives)
			{
				commandList->IASetVertexBuffers(0, 1, &primitive.m_vertexBufferView);
				commandList->IASetIndexBuffer(&primitive.m_indexBufferView);

				commandList->DrawIndexedInstanced(primitive.m_indices.size(), 1, 0, 0, 0);
			}
		}
	}
}
