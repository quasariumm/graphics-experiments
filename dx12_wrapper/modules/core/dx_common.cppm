module;

#include <d3dx12.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <wrl/client.h>

export module dx_wrapper.core.dx_common;
import dx_wrapper.core.log;

export using Microsoft::WRL::ComPtr;

export inline constexpr uint32_t max_shader_descriptors = 16384;

export void CheckHR(HRESULT hr)
{
	if (FAILED(hr))
	{
		CHAR  errMsgBuf[256];
		DWORD errMsgLen = 0;

		errMsgLen = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
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

export std::vector<char> ReadFileBinary(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path))
	{
		Log::Error("File {} nor found", path.string());
		return {};
	}
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	auto		  size = file.tellg();
	file.seekg(0);
	std::vector<char> data(size);
	file.read(data.data(), size);
	return data;
}