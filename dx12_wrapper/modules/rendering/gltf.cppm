module;

#include <d3d12.h>
#include <d3dx12.h>
#include <filesystem>
#include <fastgltf/types.hpp>

/*
 * glTF module. Handles loading models from
 */

export module dx_wrapper.rendering.gltf;
import dx_wrapper.core.dx_device;
import dx_wrapper.core.dx_common;
import glm;

export struct Vertex
{
	glm::vec3 m_position{0.f};
	glm::vec2 m_uv0{0.f};
	glm::vec2 m_uv1{0.f};
	glm::vec3 m_normal{0.f};
	glm::vec4 m_tangent{0.f};
};

export struct GltfPrimitive
{
	explicit GltfPrimitive(DxDevice& device, const fastgltf::Asset& asset, const fastgltf::Primitive& primitive);
	
	std::vector<uint32_t> m_indices{};
	std::vector<Vertex> m_vertices{};
	// GltfMaterial* m_material = nullptr;

	ComPtr<ID3D12Resource> m_vertexBuffer{};
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
	ComPtr<ID3D12Resource> m_indexBuffer{};
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};
	
private:
	void ProcessVerticesIndices(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive);
	void ProcessMaterial(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive);
	void CalculateTangents();
};

export struct GltfMesh
{
	explicit GltfMesh(DxDevice& device, const fastgltf::Asset& asset, const fastgltf::Mesh& mesh);
	
	std::vector<GltfPrimitive> m_primitives{};
};

export class GltfModel
{
public:
	explicit GltfModel(DxDevice& device, const std::filesystem::path& path);
	
	const std::vector<GltfMesh>& GetMeshes() const { return m_meshes; }
	
private:	
	std::vector<GltfMesh> m_meshes{};
};