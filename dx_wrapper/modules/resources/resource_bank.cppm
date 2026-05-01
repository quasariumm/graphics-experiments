module;

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

export module dx_wrapper.resources.resource_bank;
import dx_wrapper.resources.dx_resource;
import dx_wrapper.core.dx_common;

export class ResourceBank
{

public:

	ResourceBank() = default;

	template <typename T, typename... Args>
		requires std::is_base_of_v<DxResource, T>
	std::shared_ptr<T> GetOrCreateResource(const std::string& identifier, Args&&... args);

private:

	std::unordered_map<std::string, std::shared_ptr<DxResource>> m_resources{};
};

template <typename T, typename... Args>
	requires std::is_base_of_v<DxResource, T>
std::shared_ptr<T> ResourceBank::GetOrCreateResource(const std::string& identifier, Args&&... args)
{
	// Get
	if (m_resources.contains(identifier))
		return std::reinterpret_pointer_cast<T>(m_resources.at(identifier));
	
	// Create
	std::shared_ptr<T> res = std::make_shared<T>(std::forward<Args>(args)...);
	std::shared_ptr<DxResource> resource = std::reinterpret_pointer_cast<DxResource>(res);
	
	// Set resource name
	std::wstring widentifier = std::filesystem::path{identifier}.wstring();
	CheckHR(resource->GetResource()->SetName(widentifier.c_str()));
	
	m_resources.emplace(identifier, resource);
	return res;
}
