module;

#include <filesystem>
#include "macros.hpp"

module dx_wrapper.rendering.dx_rootsignature;
import dx_wrapper.external.directx12;
import dx_wrapper.core.log;

DxRootSignature& DxRootSignature::Add32BitConstants(const uint32_t shaderRegister, const uint32_t sizeBytes,
													const D3D12_SHADER_VISIBILITY visibility, const uint32_t space)
{
	D3D12_ROOT_PARAMETER1 param{};
	param.ParameterType			   = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	param.ShaderVisibility		   = visibility;
	param.Constants.Num32BitValues = sizeBytes / sizeof(DWORD);
	param.Constants.ShaderRegister = shaderRegister;
	param.Constants.RegisterSpace  = space;

	m_rootParameters.push_back(param);

	return *this;
}

DxRootSignature& DxRootSignature::AddConstantBuffer(const uint32_t shaderRegister, const D3D12_SHADER_VISIBILITY visibility,
													const uint32_t space)
{
	D3D12_ROOT_PARAMETER1 param{};
	param.ParameterType				= D3D12_ROOT_PARAMETER_TYPE_CBV;
	param.ShaderVisibility			= visibility;
	param.Descriptor.ShaderRegister = shaderRegister;
	param.Descriptor.RegisterSpace	= space;

	m_rootParameters.push_back(param);

	return *this;
}

DxRootSignature& DxRootSignature::AddBufferSRV(const uint32_t shaderRegister, const D3D12_SHADER_VISIBILITY visibility,
											   const uint32_t space)
{
	D3D12_ROOT_PARAMETER1 param{};
	param.ParameterType				= D3D12_ROOT_PARAMETER_TYPE_SRV;
	param.ShaderVisibility			= visibility;
	param.Descriptor.ShaderRegister = shaderRegister;
	param.Descriptor.RegisterSpace	= space;

	m_rootParameters.push_back(param);

	return *this;
}

DxRootSignature& DxRootSignature::AddBufferUAV(const uint32_t shaderRegister, const D3D12_SHADER_VISIBILITY visibility,
											   const uint32_t space)
{
	D3D12_ROOT_PARAMETER1 param{};
	param.ParameterType				= D3D12_ROOT_PARAMETER_TYPE_UAV;
	param.ShaderVisibility			= visibility;
	param.Descriptor.ShaderRegister = shaderRegister;
	param.Descriptor.RegisterSpace	= space;

	m_rootParameters.push_back(param);

	return *this;
}

DxRootSignature& DxRootSignature::AddDescriptorRange(const D3D12_DESCRIPTOR_RANGE_TYPE type, const uint32_t shaderRegister,
													 const uint32_t count, const D3D12_SHADER_VISIBILITY visibility,
													 const uint32_t space)
{
	auto* range				  = new D3D12_DESCRIPTOR_RANGE1;
	range->RangeType		  = type;
	range->NumDescriptors	  = count;
	range->BaseShaderRegister = shaderRegister;
	range->RegisterSpace	  = space;

	D3D12_ROOT_PARAMETER1 param{};
	param.ParameterType						  = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	param.ShaderVisibility					  = visibility;
	param.DescriptorTable.NumDescriptorRanges = 1;
	param.DescriptorTable.pDescriptorRanges	  = range;

	m_rootParameters.push_back(param);

	return *this;
}

DxRootSignature& DxRootSignature::AddSampler(const D3D12_FILTER filter, const D3D12_TEXTURE_ADDRESS_MODE addressMode,
											 const D3D12_STATIC_BORDER_COLOR borderColor,
											 const D3D12_SHADER_VISIBILITY	 shaderVisibility)
{
	D3D12_STATIC_SAMPLER_DESC sampler{};

	sampler.Filter			 = filter;
	sampler.AddressU		 = addressMode;
	sampler.AddressV		 = addressMode;
	sampler.AddressW		 = addressMode;
	sampler.MipLODBias		 = 0;
	sampler.MaxAnisotropy	 = 0;
	sampler.ComparisonFunc	 = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor		 = borderColor;
	sampler.MinLOD			 = 0.0f;
	sampler.MaxLOD			 = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister	 = static_cast<UINT>(m_samplers.size());
	sampler.RegisterSpace	 = 0;
	sampler.ShaderVisibility = shaderVisibility;

	m_samplers.push_back(sampler);

	return *this;
}

DxRootSignature& DxRootSignature::AddComparisonSampler(const D3D12_FILTER filter, const D3D12_TEXTURE_ADDRESS_MODE addressMode,
													   const D3D12_STATIC_BORDER_COLOR borderColor,
													   const D3D12_COMPARISON_FUNC	   comparisonFunc,
													   const D3D12_SHADER_VISIBILITY   shaderVisibility)
{
	D3D12_STATIC_SAMPLER_DESC sampler{};

	sampler.Filter			 = filter;
	sampler.AddressU		 = addressMode;
	sampler.AddressV		 = addressMode;
	sampler.AddressW		 = addressMode;
	sampler.MipLODBias		 = 0;
	sampler.MaxAnisotropy	 = 0;
	sampler.ComparisonFunc	 = comparisonFunc;
	sampler.BorderColor		 = borderColor;
	sampler.MinLOD			 = 0.0f;
	sampler.MaxLOD			 = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister	 = static_cast<UINT>(m_samplers.size());
	sampler.RegisterSpace	 = 0;
	sampler.ShaderVisibility = shaderVisibility;

	m_samplers.push_back(sampler);

	return *this;
}

void DxRootSignature::Finalize(DxDevice& device, const std::string& name, const bool heapDirectlyIndexed)
{
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion					  = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	// Allow input layout.
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	if (heapDirectlyIndexed)
		rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(static_cast<UINT>(m_rootParameters.size()),
									  m_rootParameters.data(),
									  static_cast<UINT>(m_samplers.size()),
									  m_samplers.data(),
									  rootSignatureFlags);

	// Serialize the root signature.
	ComPtr<ID3DBlob> rootSignatureBlob;
	ComPtr<ID3DBlob> errorBlob;
	if (FAILED(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
													 featureData.HighestVersion,
													 &rootSignatureBlob,
													 &errorBlob)))
	{
		const char* const ptr = static_cast<char*>(errorBlob.Get()->GetBufferPointer());

		std::string errorStr = "DirectX 12: Failed to serialize Root Signature: ";
		errorStr += ptr;

		Log::Critical("{}", errorStr);
	}

	// Create the root signature.
	CheckHR(device->CreateRootSignature(0,
										rootSignatureBlob->GetBufferPointer(),
										rootSignatureBlob->GetBufferSize(),
										IID_PPV_ARGS(m_rootSignature.GetAddressOf())));

	if (!name.empty())
	{
		std::wstring wname = std::filesystem::path{name}.wstring();
		CheckHR(m_rootSignature->SetName(wname.c_str()));
	}

	// Clear temporary storage vectors
	m_samplers.clear();
	m_rootParameters.clear();
	
	m_finalized = true;
}
