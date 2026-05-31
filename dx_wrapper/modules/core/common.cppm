module;

export module dx_wrapper.core.common;
import std;
import dx_wrapper.core.log;

export template <typename T>
	requires std::is_integral_v<T>
T NextMultipleOf(T value, T multiple)
{
	return (value + multiple - 1) & ~(multiple - 1);
}

export std::vector<char> ReadFileBinary(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path))
	{
		Log::Error("File {} not found", path.string());
		return {};
	}
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	auto		  size = file.tellg();
	file.seekg(0);
	std::vector<char> data(size);
	file.read(data.data(), size);
	return data;
}