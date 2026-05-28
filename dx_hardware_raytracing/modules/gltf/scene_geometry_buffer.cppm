module;

export module dx_hw_ray.gltf.scene_geometry_buffer;
import std;
import dx_wrapper.core.dx_common;
import dx_wrapper.core.dx_device;
import dx_wrapper.gltf.primitive;
import dx_wrapper.rendering.dx_descriptor_pile;

export class SceneGeometryBuffer
{
public:

	SceneGeometryBuffer() = default;
	~SceneGeometryBuffer();

	void AddPrimitive(const ComPtr<ID3D12Resource>& vertexBuffer, const ComPtr<ID3D12Resource>& indexBuffer);

	void Generate(DxDevice& device);
	
	DxDescriptorPile::IndexType GetVertexHeapIndex() const { return m_vertexHeapIndex; }
	DxDescriptorPile::IndexType GetIndexHeapIndex() const { return m_indexHeapIndex; }

private:

	std::vector<ComPtr<ID3D12Resource>> m_vertexBuffers;
	std::vector<ComPtr<ID3D12Resource>> m_indexBuffers;
	
	DxDescriptorPile::IndexType m_vertexHeapIndex = DxDescriptorPile::invalid;
	DxDescriptorPile::IndexType m_indexHeapIndex = DxDescriptorPile::invalid;
	std::size_t m_lastSize = 0;
};
