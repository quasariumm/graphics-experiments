module;

#include <d3d12.h>
#include <dxcapi.h>
#include <filesystem>
#include <fstream>
#include <windows.h>

module dx_wrapper.rendering.dx_shader_utils;
import dx_wrapper.core.dx_common;
import dx_wrapper.log;

namespace fs = std::filesystem;

bool RuntimeCompileShader(const fs::path& path, const std::string& type, const std::string& shaderModel,
						  const std::string& outDirExtension)
{
	const auto csoPath = fs::path{BIN_DIR} / outDirExtension / path.filename().replace_extension(std::format("{}.cso", type));

	if (fs::exists(csoPath) && fs::last_write_time(csoPath) >= fs::last_write_time(path))
		return true;

	HMODULE hDxc				= LoadLibraryW(L"dxcompiler.dll");
	auto*	pDxcCreateInstance	= (DxcCreateInstanceProc)(void*)GetProcAddress(hDxc, "DxcCreateInstance");
	HMODULE hDxil				= LoadLibraryW(L"dxil.dll");
	auto*	pDxilCreateInstance = (DxcCreateInstanceProc)(void*)GetProcAddress(hDxil, "DxcCreateInstance");

	ComPtr<IDxcCompiler3> compiler;
	CheckHR(pDxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf())));
	ComPtr<IDxcUtils> utils;
	CheckHR(pDxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf())));
	ComPtr<IDxcValidator> validator;
	CheckHR(pDxilCreateInstance(CLSID_DxcValidator, IID_PPV_ARGS(validator.GetAddressOf())));

	// Load the shader from disk
	const auto shaderBytes = ReadFileBinary(path);

	ComPtr<IDxcBlobEncoding> shaderBlob;
	CheckHR(utils->CreateBlob(shaderBytes.data(), (UINT32)shaderBytes.size(), DXC_CP_ACP, &shaderBlob));

	// Make a buffer struct
	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr	  = shaderBlob->GetBufferPointer();
	sourceBuffer.Size	  = shaderBlob->GetBufferSize();
	sourceBuffer.Encoding = 0;

	// Generate argument list
	auto includeDir = path.parent_path().wstring();

	// Note: I skip validation to do it manually later. Better error handling then
	std::wstring shaderType = fs::path{std::format("{}_{}", type, shaderModel)}.wstring();
	std::vector	 arguments	= {L"-T", shaderType.c_str(), L"-no-warnings", L"-Vd", L"-I", includeDir.c_str()};

	// Compile
	ComPtr<IDxcIncludeHandler> includeHandler;
	utils->CreateDefaultIncludeHandler(&includeHandler);

	ComPtr<IDxcResult> result;
	CheckHR(compiler->Compile(&sourceBuffer,
							  arguments.data(),
							  (UINT32)arguments.size(),
							  includeHandler.Get(),
							  IID_PPV_ARGS(result.GetAddressOf())));

	// Error Handling. Note that this will also include warnings unless disabled.
	ComPtr<IDxcBlobUtf8> pErrors;
	CheckHR(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(pErrors.GetAddressOf()), nullptr));
	if (pErrors && pErrors->GetStringLength() > 0)
	{
		Log::Error("Error(s) while runtime compiling {}:", path.string());
		std::string		   errorStr(pErrors->GetStringPointer(), pErrors->GetStringLength());
		std::istringstream stream(errorStr);
		std::string		   line;
		while (std::getline(stream, line))
			Log::Error("{}", line);
		return false;
	}

	// Get the output and write to file
	ComPtr<IDxcBlob> shaderOutput;
	CheckHR(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(shaderOutput.GetAddressOf()), nullptr));

	if (shaderOutput && shaderOutput->GetBufferSize() > 0)
	{
		// Validate
		ComPtr<IDxcOperationResult> valResult;
		CheckHR(validator->Validate(shaderOutput.Get(), DxcValidatorFlags_InPlaceEdit, &valResult));

		HRESULT valStatus = 0;
		valResult->GetStatus(&valStatus);
		if (FAILED(valStatus))
		{
			ComPtr<IDxcBlobEncoding> valErrors;
			valResult->GetErrorBuffer(&valErrors);
			if (valErrors && valErrors->GetBufferSize() > 0)
			{
				Log::Error("Validation failed for {}:", path.string());
				std::string		   errStr(static_cast<const char*>(valErrors->GetBufferPointer()), valErrors->GetBufferSize());
				std::istringstream stream(errStr);
				std::string		   line;
				while (std::getline(stream, line))
					Log::Error("{}", line);
			}
			return false;
		}

		// Write to disk
		fs::create_directories(csoPath.parent_path());
		std::ofstream file(csoPath, std::ios::out | std::ios::binary | std::ios::trunc);
		file.write(static_cast<const char*>(shaderOutput->GetBufferPointer()), shaderOutput->GetBufferSize());
	}
	fflush(stdout);

	return true;
}
