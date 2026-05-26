module;

export module dx_hw_ray.helpers.tlas;
import std;
import dx_wrapper.core.dx_common;
import dx_wrapper.core.dx_device;
import dx_wrapper.external.glm;
import dx_hw_ray.helpers.blas;

export class Tlas
{
public:

	Tlas() = default;

	// Deref operators
	ID3D12Resource* operator*() const { return m_tlas.Get(); }
	ID3D12Resource* operator->() const { return m_tlas.Get(); }
	ID3D12Resource* GetTlasResource() const { return m_tlas.Get(); }

	void AddBlas(const Blas& blas, const glm::mat4& transform, std::uint32_t instanceIndex = 0, std::uint32_t hitGroupIndex = 0);
	
	void Generate(DxDevice& device);

private:
	
	struct Instance
	{
		ID3D12Resource* m_blas;
		DirectX::XMMATRIX m_transform;
		std::uint32_t m_instanceIndex;
		std::uint32_t m_hitGroupIndex;
	};
	std::vector<Instance> m_instances;

	ComPtr<ID3D12Resource> m_tlas;
	ComPtr<ID3D12Resource> m_instanceBuffer;
};
