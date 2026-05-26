module;

export module dx_wrapper.core.dx_common;
import std;
import dx_wrapper.core.log;
export import dx_wrapper.external.win32;
export import dx_wrapper.external.directx12;

export inline constexpr std::uint32_t max_shader_descriptors = 16384;

export void CheckHR(HRESULT hr)
{
	if (FAILED(hr))
	{
		CHAR  errMsgBuf[256];
		DWORD errMsgLen = 0;

		errMsgLen = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								  nullptr,
								  hr,
								  0,
								  errMsgBuf,
								  sizeof(errMsgBuf),
								  nullptr);

		if (errMsgLen != 0)
		{
			const std::string errorStr(errMsgBuf);
			Log::Critical("{}", errorStr);
		}
		else
		{
			Log::Critical("A DirectX 12 error has occured");
		}
	}
}

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
