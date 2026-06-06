module;

export module dx_wrapper.gltf.primitive;
import std;
import dx_wrapper.external.fastgltf;
import dx_wrapper.external.directx12;
import dx_wrapper.gltf.common;
import dx_wrapper.gltf.material;
import dx_wrapper.external.glm;
import dx_wrapper.core;

export struct Vertex
{
	glm::vec3 m_position{0.f};
	float	  m_padding1;
	glm::vec2 m_uv0{0.f};
	glm::vec2 m_uv1{0.f};
	glm::vec3 m_normal{0.f};
	float	  m_padding2;
	glm::vec4 m_tangent{0.f};
};

export class GltfPrimitive
{
public:

	explicit GltfPrimitive(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
						   const fastgltf::Primitive& primitive, const MaterialsList& materials);

	virtual ~GltfPrimitive() = default;

	const std::vector<Vertex>&		  GetVertices() const;
	const std::vector<std::uint32_t>& GetIndices() const;

	const std::shared_ptr<GltfMaterial>& GetMaterial() const;

	/**
	 * @brief Binds the primitive vertex and index buffers to the Input Assembler
	 * @param device The device (used for getting the command list)
	 * @param vertexBufferSlot The slot to bind the vertex buffer to
	 */
	virtual void Bind(const DxDevice& device, std::uint32_t vertexBufferSlot) const;
	
	const ComPtr<ID3D12Resource>& GetVertexBuffer() const { return m_vertexBuffer; }
	const ComPtr<ID3D12Resource>& GetIndexBuffer() const { return m_indexBuffer; }

protected:

	std::vector<std::uint32_t>	  m_indices{};
	std::vector<Vertex>			  m_vertices{};
	std::shared_ptr<GltfMaterial> m_material = nullptr;

	ComPtr<ID3D12Resource>	 m_vertexBuffer{};
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
	ComPtr<ID3D12Resource>	 m_indexBuffer{};
	D3D12_INDEX_BUFFER_VIEW	 m_indexBufferView{};

	void ProcessVerticesIndices(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive);
	void CalculateTangents();
};
