module;

export module dx_hw_ray.gltf.scene_geometry_buffer;
import std;
import dx_wrapper.core.dx_common;
import dx_wrapper.core.dx_device;
import dx_wrapper.gltf.primitive;
import dx_wrapper.rendering.dx_descriptor_pile;
import dx_hw_ray.gltf.raytraced_model;

export class SceneGeometryBuffer
{
public:

	SceneGeometryBuffer() = default;
	~SceneGeometryBuffer();

	void AddPrimitive(const ComPtr<ID3D12Resource>& vertexBuffer, const ComPtr<ID3D12Resource>& indexBuffer);
	void AddModel(const RaytracedModel& model);

	void Generate(DxDevice& device);
	
	DxDescriptorPile::IndexType GetVertexHeapIndex() const { return m_vertexHeapIndex; }
	DxDescriptorPile::IndexType GetIndexHeapIndex() const { return m_indexHeapIndex; }
	DxDescriptorPile::IndexType GetMaterialsHeapIndex() const { return m_materialsHeapIndex; }
	DxDescriptorPile::IndexType GetMaterialIndicesHeapIndex() const { return m_materialIndicesHeapIndex; }
	DxDescriptorPile::IndexType GetBlasGeometryCountHeapIndex() const { return m_blasGeometryCountHeapIndex; }

private:
	
	DxDevice* m_device;

	std::vector<ComPtr<ID3D12Resource>> m_vertexBuffers;
	std::vector<ComPtr<ID3D12Resource>> m_indexBuffers;
	
	std::vector<ComPtr<ID3D12Resource>> m_materialsBuffers;
	std::vector<ComPtr<ID3D12Resource>> m_materialIndexBuffers;
	
	std::vector<std::uint32_t> m_blasGeometryCounts;
	ComPtr<ID3D12Resource> m_blasGeometryCountBuffer;
	
	DxDescriptorPile::IndexType m_vertexHeapIndex = DxDescriptorPile::invalid;
	DxDescriptorPile::IndexType m_indexHeapIndex = DxDescriptorPile::invalid;
	DxDescriptorPile::IndexType m_materialsHeapIndex = DxDescriptorPile::invalid;
	DxDescriptorPile::IndexType m_materialIndicesHeapIndex = DxDescriptorPile::invalid;
	DxDescriptorPile::IndexType m_blasGeometryCountHeapIndex = DxDescriptorPile::invalid;
	
	std::size_t m_lastSize = 0;
};
