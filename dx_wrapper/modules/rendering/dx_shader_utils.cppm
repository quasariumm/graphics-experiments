module;

#include <filesystem>

export module dx_wrapper.rendering.dx_shader_utils;
import dx_wrapper.core.dx_common;
import dx_wrapper.external.dxc;

/**
 * @brief Runtime compiles a shader to a .cso file and a blob. \n
 * If a .cso file already exists for the current version of the shader it is a no-op
 * @param path The path to the HLSL shader
 * @param type The type of shader ("vs", "ps", "cs", "lib")
 * @param shaderModel The shader model to use (e.g. "6_6")
 * @param outDirExtension [optional, default = ""] Defines whether the shader needs to be placed in a subdirectory of the output
 * dir
 * @returns A blob of the shader output
 */
export std::optional<ComPtr<IDxcBlob>> RuntimeCompileShader(const std::filesystem::path& path, const std::string& type,
															const std::string& shaderModel,
															const std::string& outDirExtension = "");
