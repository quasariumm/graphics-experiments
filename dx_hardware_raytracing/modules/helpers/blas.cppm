module;

export module dx_hw_ray.helpers.blas;
import std;
import dx_wrapper.external.glm;
import dx_wrapper.external.directx12;
import dx_wrapper.core.dx_common;
import dx_wrapper.core.dx_device;
import dx_hw_ray.external.dxr;

export class Blas
{
public:

	Blas() = default;

	// Deref operators
	ID3D12Resource* operator*() const { return m_blas.Get(); }
	ID3D12Resource* operator->() const { return m_blas.Get(); }
	ID3D12Resource* GetBlasResource() const { return m_blas.Get(); }

	void AddGeometry(const DxDevice& device, const glm::mat4& transform, ID3D12Resource* vertexBuffer, std::size_t vertexStride,
					 std::uint32_t vertexCount, ID3D12Resource* indexBuffer, std::uint32_t indexCount,
					 DXGI_FORMAT					 indexFormat	 = DXGI_FORMAT_R32_UINT,
					 D3D12_RAYTRACING_GEOMETRY_FLAGS additionalFlags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE);

	void Generate(DxDevice& device);

private:

	/** Why does a BLAS need GPUVAs? */
	std::vector<ComPtr<ID3D12Resource>>			m_transforms;
	std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> m_geometries;

	ComPtr<ID3D12Resource> m_blas = nullptr;
};
