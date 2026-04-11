module;

#include <d3d12.h>
#include <vector>
#include <fastgltf/types.hpp>

export module dx_wrapper.gltf.primitive;
import dx_wrapper.gltf.material;
import dx_wrapper.external.glm;
import dx_wrapper.core;

export struct Vertex
{
	glm::vec3 m_position{0.f};
	glm::vec2 m_uv0{0.f};
	glm::vec2 m_uv1{0.f};
	glm::vec3 m_normal{0.f};
	glm::vec4 m_tangent{0.f};
};

export class GltfPrimitive
{

public:

	explicit GltfPrimitive(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
						   const fastgltf::Primitive& primitive);

	const std::vector<Vertex>&	 GetVertices() const;
	const std::vector<uint32_t>& GetIndices() const;

	const std::optional<GltfMaterial>& GetMaterial() const;

	/**
	 * @brief Binds the primitive vertex and index buffers to the Input Assembler
	 * @param device The device (used for getting the command list)
	 * @param vertexBufferSlot The slot to bind the vertex buffer to
	 */
	void Bind(const DxDevice& device, uint32_t vertexBufferSlot) const;

private:

	std::vector<uint32_t>		m_indices{};
	std::vector<Vertex>			m_vertices{};
	std::optional<GltfMaterial> m_material{};

	ComPtr<ID3D12Resource>	 m_vertexBuffer{};
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
	ComPtr<ID3D12Resource>	 m_indexBuffer{};
	D3D12_INDEX_BUFFER_VIEW	 m_indexBufferView{};

	void ProcessVerticesIndices(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive);
	void CalculateTangents();
};
