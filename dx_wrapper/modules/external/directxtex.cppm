module;

#include <d3d12.h>

#include <DirectXTex.h>
#include <../Auxiliary/DirectXTexEXR.h>

export module dx_wrapper.external.directxtex;

export namespace DirectX
{
using DirectX::ScratchImage;
using DirectX::Image;
using DirectX::TexMetadata;

using DirectX::LoadFromDDSFile;
using DirectX::LoadFromDDSFileEx;
using DirectX::LoadFromDDSMemory;
using DirectX::LoadFromDDSMemoryEx;

using DirectX::LoadFromWICFile;
using DirectX::LoadFromWICMemory;

using DirectX::LoadFromHDRFile;
using DirectX::LoadFromHDRMemory;

using DirectX::LoadFromEXRFile;
using DirectX::LoadFromEXRMemory;

using DirectX::GenerateMipMaps;
using DirectX::GenerateMipMaps3D;
using DirectX::CalculateMipLevels;
using DirectX::CalculateMipLevels3D;

using DirectX::CreateTexture;
using DirectX::CreateTextureEx;

using DirectX::PrepareUpload;

using DirectX::TEX_DIMENSION;
using DirectX::TEX_FILTER_FLAGS;
using DirectX::TEX_MISC_FLAG;
using DirectX::TEX_MISC_TEXTURECUBE;
using DirectX::CREATETEX_FLAGS;
using DirectX::DDS_FLAGS;
using DirectX::WIC_FLAGS;
}