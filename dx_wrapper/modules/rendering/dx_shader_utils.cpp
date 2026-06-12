module;

module dx_wrapper.rendering.dx_shader_utils;
import std;
import dx_wrapper.core.dx_common;
import dx_wrapper.core.log;
import dx_wrapper.external.dxc;

namespace Filesystem = std::filesystem;

std::optional<ComPtr<IDxcBlob>> RuntimeCompileShader(const Filesystem::path& path, const std::string& type,
													 const std::string& shaderModel, const std::string& outDirExtension)
{
	if (path.empty())
		return std::nullopt;

	const auto csoPath = path.parent_path() / outDirExtension / path.filename().replace_extension(std::format("{}.cso", type));

	std::error_code ec1{}, ec2{};
	auto csoWriteTime = Filesystem::last_write_time(csoPath, ec1);
	auto hlslWriteTime = Filesystem::last_write_time(path, ec2);
	
	if (Filesystem::exists(csoPath) && csoWriteTime >= hlslWriteTime)
	{
		const auto data = ReadFileBinary(csoPath);

		HMODULE hDxc			   = LoadLibrary("dxcompiler.dll");
		auto*	pDxcCreateInstance = (DxcCreateInstanceProc)(void*)GetProcAddress(hDxc, "DxcCreateInstance");

		ComPtr<IDxcUtils> utils;
		CheckHR(pDxcCreateInstance(dxc::CLSID_DxcUtils, GetIID(utils), GetPPV(utils)));

		ComPtr<IDxcBlobEncoding> blobEncoding;
		CheckHR(utils->CreateBlob(data.data(), data.size(), DXC_CP_ACP, blobEncoding.GetAddressOf()));
		return blobEncoding;
	}

	HMODULE hDxc				= LoadLibrary("dxcompiler.dll");
	auto*	pDxcCreateInstance	= (DxcCreateInstanceProc)(void*)GetProcAddress(hDxc, "DxcCreateInstance");
	HMODULE hDxil				= LoadLibrary("dxil.dll");
	auto*	pDxilCreateInstance = (DxcCreateInstanceProc)(void*)GetProcAddress(hDxil, "DxcCreateInstance");

	ComPtr<IDxcCompiler3> compiler;
	CheckHR(pDxcCreateInstance(dxc::CLSID_DxcCompiler, GetIID(compiler), GetPPV(compiler)));
	ComPtr<IDxcUtils> utils;
	CheckHR(pDxcCreateInstance(dxc::CLSID_DxcUtils, GetIID(utils), GetPPV(utils)));
	ComPtr<IDxcValidator> validator;
	CheckHR(pDxilCreateInstance(dxc::CLSID_DxcValidator, GetIID(validator), GetPPV(validator)));

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
	std::wstring shaderType = Filesystem::path{std::format("{}_{}", type, shaderModel)}.wstring();
	std::vector	 arguments	= {L"-T", shaderType.c_str(), L"-no-warnings", L"-Zi", L"-Qembed_debug", L"-I", includeDir.c_str()};

	// Compile
	ComPtr<IDxcIncludeHandler> includeHandler;
	CheckHR(utils->CreateDefaultIncludeHandler(&includeHandler));

	ComPtr<IDxcResult> result;
	CheckHR(compiler->Compile(&sourceBuffer,
							  arguments.data(),
							  (UINT32)arguments.size(),
							  includeHandler.Get(),
							  GetIID(result), GetPPV(result)));

	// Error Handling. Note that this will also include warnings unless disabled.
	ComPtr<IDxcBlobUtf8> pErrors;
	CheckHR(result->GetOutput(DXC_OUT_ERRORS, GetIID(pErrors), GetPPV(pErrors), nullptr));
	if (pErrors && pErrors->GetStringLength() > 0)
	{
		Log::Error("Error(s) while runtime compiling {}:", path.string());
		std::string		   errorStr(pErrors->GetStringPointer(), pErrors->GetStringLength());
		std::istringstream stream(errorStr);
		std::string		   line;
		while (std::getline(stream, line))
			Log::Error("{}", line);
		std::cout.flush();
		return std::nullopt;
	}

	// Get the output and write to file
	ComPtr<IDxcBlob> shaderOutput;
	CheckHR(result->GetOutput(DXC_OUT_OBJECT, GetIID(shaderOutput), GetPPV(shaderOutput), nullptr));

	if (shaderOutput && shaderOutput->GetBufferSize() > 0)
	{
		// Validate
		ComPtr<IDxcOperationResult> valResult;
		CheckHR(validator->Validate(shaderOutput.Get(), DxcValidatorFlagsInPlaceEdit, &valResult));

		HRESULT valStatus = 0;
		CheckHR(valResult->GetStatus(&valStatus));
		if (FAILED(valStatus))
		{
			ComPtr<IDxcBlobEncoding> valErrors;
			CheckHR(valResult->GetErrorBuffer(&valErrors));
			if (valErrors && valErrors->GetBufferSize() > 0)
			{
				Log::Error("Validation failed for {}:", path.string());
				std::string		   errStr(static_cast<const char*>(valErrors->GetBufferPointer()), valErrors->GetBufferSize());
				std::istringstream stream(errStr);
				std::string		   line;
				while (std::getline(stream, line))
					Log::Error("{}", line);
			}
			std::cout.flush();
			return std::nullopt;
		}

		// Write to disk
		Filesystem::create_directories(csoPath.parent_path());
		std::ofstream file(csoPath, std::ios::out | std::ios::binary | std::ios::trunc);
		file.write(static_cast<const char*>(shaderOutput->GetBufferPointer()), shaderOutput->GetBufferSize());
	}
	std::cout.flush();

	return shaderOutput;
}
