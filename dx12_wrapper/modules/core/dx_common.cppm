module;

#include <string>
#include <wrl/client.h>

export module core.dx_common;
import log;

using Microsoft::WRL::ComPtr;

export void CheckHR(HRESULT hr)
{
	if (FAILED(hr))
	{
		CHAR errMsgBuf[256];
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