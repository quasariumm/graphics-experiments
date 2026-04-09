module;

#include <d3d12.h>
#include <filesystem>
#include <variant>
#include <wrl/client.h>

export module dx_wrapper.rendering.dx_pipelinestate;
export import dx_wrapper.rendering.dx_rootsignature;
import dx_wrapper.core.dx_device;

namespace fs = std::filesystem;
using Microsoft::WRL::ComPtr;

export class DxPipelineState
{
public:

	DxPipelineState() = default;

	ID3D12PipelineState* operator*() const { return m_pipelineState.Get(); }

	/*
	 * Shader Setters
	 */

	// Geometry pipeline
	DxPipelineState& SetVertexShader(const std::filesystem::path& path);
	DxPipelineState& SetHullShader(const std::filesystem::path& path);
	DxPipelineState& SetDomainShader(const std::filesystem::path& path);
	DxPipelineState& SetGeometryShader(const std::filesystem::path& path);
	DxPipelineState& SetPixelShader(const std::filesystem::path& path);

	// Compute pipeline
	DxPipelineState& SetComputeShader(const std::filesystem::path& path);

	// Mesh pipeline
	DxPipelineState& SetAmplificationShader(const std::filesystem::path& path);
	DxPipelineState& SetMeshShader(const std::filesystem::path& path);
	DxPipelineState& SetMeshPixelShader(const std::filesystem::path& path);

	/*
	 * Input / Output
	 */

	DxPipelineState& AddVertexInput(const std::string& semantic, DXGI_FORMAT format, uint32_t semanticIndex = 0,
									uint32_t				   inputSlot		= 0,
									D3D12_INPUT_CLASSIFICATION classification	= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
									uint32_t				   instanceStepRate = 0);

	DxPipelineState& AddRenderTarget(DXGI_FORMAT format);
	DxPipelineState& SetDepthRenderTarget(DXGI_FORMAT format);

	/*
	 * Pipeline State
	 */

	DxPipelineState& SetBlending(bool blend);
	DxPipelineState& SetCullMode(D3D12_CULL_MODE mode);
	DxPipelineState& SetDepthTesting(bool testDepth);
	DxPipelineState& SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);

	void Finalize(DxDevice& device, const DxRootSignature& rootSignature, const std::string& name = "",
				  const std::string& outDirExtension = "");

private:

	struct GraphicsPipeline
	{
		fs::path m_vertexShader;
		fs::path m_domainShader;
		fs::path m_hullShader;
		fs::path m_geometryShader;
		fs::path m_pixelShader;
	};

	struct MeshPipeline
	{
		fs::path m_amplificationShader;
		fs::path m_meshShader;
		fs::path m_pixelShader;
	};

	using ComputePipeline = fs::path;

	std::variant<GraphicsPipeline, ComputePipeline, MeshPipeline> m_shaders;

	struct VertexInput
	{
		std::string				   m_semantic;
		DXGI_FORMAT				   m_format;
		uint32_t				   m_semanticIndex;
		uint32_t				   m_inputSlot;
		D3D12_INPUT_CLASSIFICATION m_classification;
		uint32_t				   m_instanceStepRate;
	};
	std::vector<VertexInput> m_vertexInputs{};

	std::vector<DXGI_FORMAT> m_renderTargets{};
	DXGI_FORMAT				 m_depthFormat = DXGI_FORMAT_UNKNOWN;

	struct Params
	{
		bool						  m_blend			  = false;
		D3D12_CULL_MODE				  m_cullMode		  = D3D12_CULL_MODE_BACK;
		bool						  m_depthTesting	  = true;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE m_primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	};
	Params m_params{};

	ComPtr<ID3D12PipelineState> m_pipelineState{};
};
