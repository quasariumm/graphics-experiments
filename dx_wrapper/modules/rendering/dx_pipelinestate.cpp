module;

#include <filesystem>
#include <format>
#include <variant>
#include "macros.hpp"

module dx_wrapper.rendering.dx_pipelinestate;
import dx_wrapper.core.dx_common;
import dx_wrapper.rendering.dx_shader_utils;
import dx_wrapper.external.dxc;

template <class... Ts>
struct Overload : Ts...
{
	using Ts::operator()...;
};

D3D12_SHADER_BYTECODE GetBytecode(const ComPtr<IDxcBlob>& blob)
{
	if (!blob)
		return CD3DX12_SHADER_BYTECODE{};
	return CD3DX12_SHADER_BYTECODE{blob->GetBufferPointer(), blob->GetBufferSize()};
}

DxPipelineState& DxPipelineState::SetVertexShader(const std::filesystem::path& path)
{
	if (auto* gp = std::get_if<GraphicsPipeline>(&m_shaders))
		gp->m_vertexShader = std::move(path);
	else
		m_shaders = GraphicsPipeline{.m_vertexShader = std::move(path)};
	return *this;
}

DxPipelineState& DxPipelineState::SetHullShader(const std::filesystem::path& path)
{
	if (auto* gp = std::get_if<GraphicsPipeline>(&m_shaders))
		gp->m_hullShader = std::move(path);
	else
		m_shaders = GraphicsPipeline{.m_hullShader = std::move(path)};
	return *this;
}

DxPipelineState& DxPipelineState::SetDomainShader(const std::filesystem::path& path)
{
	if (auto* gp = std::get_if<GraphicsPipeline>(&m_shaders))
		gp->m_domainShader = std::move(path);
	else
		m_shaders = GraphicsPipeline{.m_domainShader = std::move(path)};
	return *this;
}

DxPipelineState& DxPipelineState::SetGeometryShader(const std::filesystem::path& path)
{
	if (auto* gp = std::get_if<GraphicsPipeline>(&m_shaders))
		gp->m_geometryShader = std::move(path);
	else
		m_shaders = GraphicsPipeline{.m_geometryShader = std::move(path)};
	return *this;
}

DxPipelineState& DxPipelineState::SetPixelShader(const std::filesystem::path& path)
{
	if (auto* gp = std::get_if<GraphicsPipeline>(&m_shaders))
		gp->m_pixelShader = std::move(path);
	else
		m_shaders = GraphicsPipeline{.m_pixelShader = std::move(path)};
	return *this;
}

DxPipelineState& DxPipelineState::SetComputeShader(const std::filesystem::path& path)
{
	if (auto* gp = std::get_if<ComputePipeline>(&m_shaders))
		*gp = std::move(path);
	else
		m_shaders = std::move(path);
	return *this;
}

DxPipelineState& DxPipelineState::SetAmplificationShader(const std::filesystem::path& path)
{
	if (auto* gp = std::get_if<MeshPipeline>(&m_shaders))
		gp->m_amplificationShader = std::move(path);
	else
		m_shaders = MeshPipeline{.m_amplificationShader = std::move(path)};
	return *this;
}

DxPipelineState& DxPipelineState::SetMeshShader(const std::filesystem::path& path)
{
	if (auto* gp = std::get_if<MeshPipeline>(&m_shaders))
		gp->m_meshShader = std::move(path);
	else
		m_shaders = MeshPipeline{.m_meshShader = std::move(path)};
	return *this;
}

DxPipelineState& DxPipelineState::SetMeshPixelShader(const std::filesystem::path& path)
{
	if (auto* gp = std::get_if<MeshPipeline>(&m_shaders))
		gp->m_pixelShader = std::move(path);
	else
		m_shaders = MeshPipeline{.m_pixelShader = std::move(path)};
	return *this;
}

DxPipelineState& DxPipelineState::AddVertexInput(const std::string& semantic, DXGI_FORMAT format, uint32_t semanticIndex,
												 uint32_t inputSlot, D3D12_INPUT_CLASSIFICATION classification,
												 uint32_t instanceStepRate)
{
	m_vertexInputs.emplace_back(semantic, format, semanticIndex, inputSlot, classification, instanceStepRate);
	return *this;
}

DxPipelineState& DxPipelineState::AddRenderTarget(DXGI_FORMAT format)
{
	m_renderTargets.emplace_back(format);
	return *this;
}

DxPipelineState& DxPipelineState::SetDepthRenderTarget(const DXGI_FORMAT format)
{
	m_depthFormat = format;
	return *this;
}

DxPipelineState& DxPipelineState::SetBlending(const bool blend)
{
	m_params.m_blend = blend;
	return *this;
}

DxPipelineState& DxPipelineState::SetCullMode(const D3D12_CULL_MODE mode)
{
	m_params.m_cullMode = mode;
	return *this;
}

DxPipelineState& DxPipelineState::SetDepthTesting(const bool testDepth)
{
	m_params.m_depthTesting = testDepth;
	return *this;
}

DxPipelineState& DxPipelineState::SetPrimitiveTopology(const D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
{
	m_params.m_primitiveTopology = type;
	return *this;
}

void DxPipelineState::Finalize(DxDevice& device, const DxRootSignature& rootSignature, const std::string& name,
							   const std::string& outDirExtension)
{
	// Create Input Layout
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

	for (const VertexInput& vertexInput : m_vertexInputs)
	{
		D3D12_INPUT_ELEMENT_DESC desc = {};

		desc.SemanticName		  = vertexInput.m_semantic.c_str();
		desc.SemanticIndex		  = vertexInput.m_semanticIndex;
		desc.Format				  = vertexInput.m_format;
		desc.InputSlot			  = vertexInput.m_inputSlot;
		desc.AlignedByteOffset	  = D3D12_APPEND_ALIGNED_ELEMENT;
		desc.InputSlotClass		  = vertexInput.m_classification;
		desc.InstanceDataStepRate = vertexInput.m_instanceStepRate;

		inputLayout.push_back(desc);
	}

	std::visit(
			Overload{[&](const GraphicsPipeline& shaders)
					 {
						 const ComPtr<IDxcBlob> vsBlob =
								 RuntimeCompileShader(shaders.m_vertexShader, "vs", "6_6", outDirExtension).value_or(nullptr);
						 const ComPtr<IDxcBlob> hsBlob =
								 RuntimeCompileShader(shaders.m_hullShader, "hs", "6_6", outDirExtension).value_or(nullptr);
						 const ComPtr<IDxcBlob> dsBlob =
								 RuntimeCompileShader(shaders.m_domainShader, "ds", "6_6", outDirExtension).value_or(nullptr);
						 const ComPtr<IDxcBlob> gsBlob =
								 RuntimeCompileShader(shaders.m_geometryShader, "hs", "6_6", outDirExtension).value_or(nullptr);
						 const ComPtr<IDxcBlob> psBlob =
								 RuntimeCompileShader(shaders.m_pixelShader, "ps", "6_6", outDirExtension).value_or(nullptr);

						 // Graphics pipeline (Vertex -> [Hull] -> [Domain] -> [Geometry] -> [Pixel])
						 D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
						 psoDesc.pRootSignature = *rootSignature;
						 psoDesc.InputLayout	= {inputLayout.data(), static_cast<UINT>(inputLayout.size())};

						 // Shaders
						 psoDesc.VS = GetBytecode(vsBlob);
						 psoDesc.HS = GetBytecode(hsBlob);
						 psoDesc.DS = GetBytecode(dsBlob);
						 psoDesc.GS = GetBytecode(gsBlob);
						 psoDesc.PS = GetBytecode(psBlob);
						 // Other
						 psoDesc.RasterizerState		  = CD3DX12_RASTERIZER_DESC{D3D12_DEFAULT};
						 psoDesc.RasterizerState.CullMode = m_params.m_cullMode;

						 psoDesc.BlendState = CD3DX12_BLEND_DESC{D3D12_DEFAULT};
						 if (m_params.m_blend)
						 {
							 psoDesc.BlendState.AlphaToCoverageEnable				  = TRUE;
							 psoDesc.BlendState.IndependentBlendEnable				  = FALSE;
							 psoDesc.BlendState.RenderTarget[0].BlendEnable			  = TRUE;
							 psoDesc.BlendState.RenderTarget[0].SrcBlend			  = D3D12_BLEND_SRC_ALPHA;
							 psoDesc.BlendState.RenderTarget[0].DestBlend			  = D3D12_BLEND_INV_SRC_ALPHA;
							 psoDesc.BlendState.RenderTarget[0].BlendOp				  = D3D12_BLEND_OP_ADD;
							 psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
						 }

						 psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{D3D12_DEFAULT};

						 if (!m_params.m_depthTesting)
							 psoDesc.DepthStencilState.DepthEnable = FALSE;

						 psoDesc.SampleMask			   = UINT_MAX;
						 psoDesc.PrimitiveTopologyType = m_params.m_primitiveTopology;

						 psoDesc.NumRenderTargets = static_cast<UINT>(m_renderTargets.size());
						 memcpy(&psoDesc.RTVFormats[0], m_renderTargets.data(), m_renderTargets.size() * sizeof(DXGI_FORMAT));

						 psoDesc.DSVFormat			= m_depthFormat;
						 psoDesc.SampleDesc.Count	= 1;
						 psoDesc.SampleDesc.Quality = 0;

						 CheckHR(device.GetDXDevice()->CreateGraphicsPipelineState(
								 &psoDesc,
								 IID_PPV_ARGS(m_pipelineState.GetAddressOf())));
					 },
					 [&](const ComputePipeline& shader)
					 {
						 const ComPtr<IDxcBlob> csBlob =
								 RuntimeCompileShader(shader, "cs", "6_6", outDirExtension).value_or(nullptr);

						 D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
						 psoDesc.pRootSignature = *rootSignature;
						 psoDesc.CS				= GetBytecode(csBlob);

						 CheckHR(device.GetDXDevice()->CreateComputePipelineState(
								 &psoDesc,
								 IID_PPV_ARGS(m_pipelineState.GetAddressOf())));
					 },
					 [&](const MeshPipeline& shaders)
					 {
						 const ComPtr<IDxcBlob> asBlob =
								 RuntimeCompileShader(shaders.m_amplificationShader, "as", "6_6", outDirExtension)
										 .value_or(nullptr);
						 const ComPtr<IDxcBlob> msBlob =
								 RuntimeCompileShader(shaders.m_meshShader, "ms", "6_6", outDirExtension).value_or(nullptr);
						 const ComPtr<IDxcBlob> psBlob =
								 RuntimeCompileShader(shaders.m_pixelShader, "hs", "6_6", outDirExtension).value_or(nullptr);

						 D3DX12_MESH_SHADER_PIPELINE_STATE_DESC psoDesc{};

						 psoDesc.pRootSignature = *rootSignature;
						 psoDesc.AS				= GetBytecode(asBlob);
						 psoDesc.MS				= GetBytecode(msBlob);
						 psoDesc.PS				= GetBytecode(psBlob);

						 D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = {};
						 streamDesc.SizeInBytes						 = sizeof(psoDesc);
						 streamDesc.pPipelineStateSubobjectStream	 = &psoDesc;

						 CheckHR(device.GetDXDevice()->CreatePipelineState(&streamDesc,
																		   IID_PPV_ARGS(m_pipelineState.GetAddressOf())));
					 }},
			m_shaders);

	if (!name.empty())
	{
		std::wstring wname = Filesystem::path{name}.wstring();
		CheckHR(m_pipelineState->SetName(wname.c_str()));
	}

	// Clear temporary storage vectors
	m_shaders = GraphicsPipeline{};
	m_vertexInputs.clear();
	m_renderTargets.clear();
	m_depthFormat = DXGI_FORMAT_UNKNOWN;
}
