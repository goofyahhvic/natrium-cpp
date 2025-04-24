#include "Pch.hpp"
#include "Natrium-Renderer/Shader.hpp"

#include "Natrium-Core/Logger.hpp"
#include "Natrium-Core/Context.hpp"
#include <shaderc/shaderc.hpp>

namespace Na {
	static std::filesystem::path shaderOutputDir;

	static void compileShader(const std::filesystem::path& input_path, const std::string_view& entry_point)
	{
	#if defined(NA_PLATFORM_WINDOWS)
		#define C_STR string().c_str
	#elif defined(NA_PLATFORM_LINUX)
		#define C_STR c_str
	#else
		#define C_STR c_str
	#endif // NA_PLATFORM

		if (!std::filesystem::exists(input_path))
		{
			g_Logger.fmt(Warn, "Shader source file {} doesnt exist!", input_path.C_STR());
			return;
		}

		if (shaderOutputDir.empty())
		{
			shaderOutputDir = Context::GetExecPath().parent_path();
			g_Logger.fmt(Warn, "No shader output directory defined! Using {}", shaderOutputDir.C_STR());
		}
		std::filesystem::path output_path = shaderOutputDir / input_path.filename().replace_extension(".spv");

		bool output_exists = std::filesystem::exists(output_path);

		if (output_exists &&
			std::filesystem::last_write_time(output_path)
				> std::filesystem::last_write_time(input_path))
			return;

		std::ifstream input_file(input_path, std::ios::ate | std::ios::binary);
		if (input_file.fail() || !input_file.good())
		{
			throw std::runtime_error(NA_FORMAT("Failed to open file {}", input_path.string()));
		}

		u64 input_size = (u64)input_file.tellg();
		Byte* input_data = tmalloc<Byte>(input_size);

		input_file.seekg(0);
		input_file.read((char*)input_data, input_size);
		input_file.close();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

	#if !defined(NA_CONFIG_DEBUG)
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
	#endif

		shaderc::SpvCompilationResult spv = compiler.CompileGlslToSpv(
			(const char*)input_data,
			input_size,
			shaderc_glsl_infer_from_source,
			input_path.C_STR(),
			entry_point.data(),
			options
		);
		free(input_data);

		if (g_Logger.enabled())
		{
			if (spv.GetCompilationStatus() == shaderc_compilation_status_success)
			{
				if (spv.GetNumWarnings())
				{
					g_Logger.fmt(
						Na::Warn,
						"Compiled {} to {} with {} warnings!",
							input_path.C_STR(),
							output_path.C_STR(),
							spv.GetNumWarnings()
					);
				} else
				{
					g_Logger.fmt(
						Info,
						"Compiled {} to {} with 0 warnings!",
							input_path.C_STR(),
							output_path.C_STR()
					);
				}
			} else
			{
				g_Logger.fmt(
					Na::Error,
					"Failed to compile {} with {} errors and {} warnings!",
						input_path.C_STR(),
						spv.GetNumErrors(),
						spv.GetNumWarnings()
				);
				g_Logger.log(Na::Error, spv.GetErrorMessage());
			}
		}

		u64 output_size = std::distance(spv.cbegin(), spv.cend()) * sizeof(u32);

		std::ofstream output_file(output_path, std::ios::binary);
		if (!output_file.is_open())
			throw std::runtime_error(NA_FORMAT("Failed to open file {} ", output_path.C_STR()));

		output_file.write((const char*)spv.cbegin(), output_size);
		output_file.close();
	}

	static ArrayVector<Byte> loadShader(const std::filesystem::path& shader_filename)
	{
		std::filesystem::path input_path = shaderOutputDir / shader_filename;
		std::ifstream file(input_path, std::ios::ate | std::ios::binary);
		if (!file.is_open())
			throw std::runtime_error(NA_FORMAT("Failed to open file {} ", input_path.C_STR()));

		u64 size = file.tellg();
		ArrayVector<Byte> file_data(size);

		file.seekg(0);
		file.read((char*)file_data.ptr(), size);
		file.close();

		return file_data;
	#undef C_STR
	}

	static vk::ShaderModule createShaderModule(
		const std::filesystem::path& shader_file,
		const std::string_view& entry_point
	)
	{
		vk::ShaderModuleCreateInfo create_info;

		compileShader(shader_file, entry_point);
		ArrayVector<Byte> code = loadShader(shader_file.filename().replace_extension(".spv"));
		while (code.size() % 4)
			code.emplace(0);

		create_info.codeSize = code.size();
		create_info.pCode = (u32*)code.ptr();

		return VkContext::GetLogicalDevice().createShaderModule(create_info);
	}

	Shader::Shader(const std::filesystem::path& path, ShaderStageBits stage, const std::string_view& entry_point)
	: stage(stage),
	entry_point(entry_point),
	module(createShaderModule(path, entry_point))
	{}

	void Shader::destroy(void)
	{
		if (!this->module)
			return;

		VkContext::GetLogicalDevice().destroyShaderModule(this->module);
		this->module = nullptr;
	}

} // namespace Na
