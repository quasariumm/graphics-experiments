module;

export module dx_hw_ray.helpers.blas;
import std;
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

	void GenerateTriangles(DxDevice& device, ID3D12Resource* vertexBuffer, std::size_t vertexStride, std::uint32_t vertexCount,
						   ID3D12Resource* indexBuffer, std::uint32_t indexCount,
						   DXGI_FORMAT					   indexFormat	   = DXGI_FORMAT_R32_UINT,
						   D3D12_RAYTRACING_GEOMETRY_FLAGS additionalFlags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE);

private:

	ComPtr<ID3D12Resource> m_blas = nullptr;
};
