module;

#include <d3d12.h>
#include <string>
#include <vector>

export module dx_wrapper.rendering.dx_rootsignature;
import dx_wrapper.core.dx_common;
import dx_wrapper.core.dx_device;

export class DxRootSignature
{
public:

	DxRootSignature() = default;

	[[nodiscard]] ID3D12RootSignature* GetRootSignature() const { return m_rootSignature.Get(); }
	ID3D12RootSignature*			   operator*() const { return m_rootSignature.Get(); }

	/*
	 * Root Signature entries
	 */
	DxRootSignature& Add32BitConstants(uint32_t shaderRegister, uint32_t sizeBytes,
									   D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, uint32_t space = 0);

	DxRootSignature& AddConstantBuffer(uint32_t				   shaderRegister,
									   D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, uint32_t space = 0);

	DxRootSignature& AddBufferSRV(uint32_t shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL,
								  uint32_t space = 0);
	DxRootSignature& AddBufferUAV(uint32_t shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL,
								  uint32_t space = 0);

	DxRootSignature& AddDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, uint32_t shaderRegister, uint32_t count,
										D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, uint32_t space = 0);

	/*
	 * Samplers
	 */
	DxRootSignature& AddSampler(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addressMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
								D3D12_STATIC_BORDER_COLOR borderColor	   = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
								D3D12_SHADER_VISIBILITY	  shaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL);

	DxRootSignature& AddComparisonSampler(D3D12_FILTER				 filter,
										  D3D12_TEXTURE_ADDRESS_MODE addressMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
										  D3D12_STATIC_BORDER_COLOR	 borderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
										  D3D12_COMPARISON_FUNC		 comparisonFunc	  = D3D12_COMPARISON_FUNC_LESS_EQUAL,
										  D3D12_SHADER_VISIBILITY	 shaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL);


	void Finalize(DxDevice& device, const std::string& name, bool heapDirectlyIndexed = false);

private:

	ComPtr<ID3D12RootSignature> m_rootSignature;

	std::vector<D3D12_STATIC_SAMPLER_DESC> m_samplers;
	std::vector<D3D12_ROOT_PARAMETER1>	   m_rootParameters;
};
