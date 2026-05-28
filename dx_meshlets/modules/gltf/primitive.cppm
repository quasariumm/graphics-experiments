module;

export module dx_meshlets.gltf.primitive;
import std;
import dx_wrapper.external.fastgltf;
import dx_wrapper.external.directx12;
import dx_meshlets.external.meshopt_meshlets;
import dx_wrapper.gltf.material;
import dx_wrapper.external.glm;
import dx_wrapper.core;
import dx_wrapper.rendering.dx_descriptor_heap;
import dx_wrapper.rendering.dx_descriptor_pile;

export struct Vertex
{
	glm::vec3 m_position{0.f};
	float m_padding;
	glm::vec2 m_uv0{0.f};
	glm::vec2 m_uv1{0.f};
	glm::vec3 m_normal{0.f};
	float m_padding2;
	glm::vec4 m_tangent{0.f};
};

export class GltfPrimitive
{

public:

	explicit GltfPrimitive(DxDevice& device, const std::filesystem::path& modelPath, const fastgltf::Asset& asset,
						   const fastgltf::Primitive& primitive);

	const std::vector<Vertex>&		  GetVertices() const;
	const std::vector<std::uint32_t>& GetIndices() const;
	std::size_t						  GetMeshletCount() const { return m_meshlets.size(); }

	const std::optional<GltfMaterial>& GetMaterial() const;

	void Bind(const DxDevice& device, std::uint32_t descTableRootSigParam) const;

private:

	std::vector<std::uint32_t>	m_indices{};
	std::vector<Vertex>			m_vertices{};
	std::optional<GltfMaterial> m_material{};

	ComPtr<ID3D12Resource>	 m_vertexBuffer{};
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};

	// Meshlets
	std::vector<Meshopt::Meshlet> m_meshlets{};
	std::vector<std::uint32_t>	  m_meshletVertices{};
	std::vector<std::uint32_t>	  m_meshletTriangles{};

	DxDescriptorHeap m_descriptorHeap{};
	
	ComPtr<ID3D12Resource> m_meshletsBuffer{};
	ComPtr<ID3D12Resource> m_meshletVerticesBuffer{};
	ComPtr<ID3D12Resource> m_meshletTrianglesBuffer{};
	
	DxDescriptorPile::IndexType m_descriptorTableHeapIndex{};
	D3D12_GPU_DESCRIPTOR_HANDLE m_descriptorTableGpuHandle{};

	void ProcessVerticesIndices(const fastgltf::Asset& asset, const fastgltf::Primitive& primitive);
	void CreateMeshlets();
	void CalculateTangents();
};
