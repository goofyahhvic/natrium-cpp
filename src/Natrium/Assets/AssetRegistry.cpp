#include "Pch.hpp"
#include "Natrium/Assets/AssetRegistry.hpp"

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

namespace Na {
	extern ArrayVector<u32> LoadSpv(const std::filesystem::path& path);

	AssetRegistry::AssetRegistry(
		const std::filesystem::path& asset_dir,
		const std::filesystem::path& shader_output_dir
	)
	: m_AssetDir(asset_dir),
	m_ShaderOutputDir(shader_output_dir)
	{}

	void AssetRegistry::destroy(void)
	{
		m_Assets.clear();
		m_AssetDir.clear();
		m_ShaderOutputDir.clear();
	}

	ShaderModule AssetRegistry::create_shader_module_from_src(
		const std::string_view& src_path,
		ShaderStageBits stage,
		const std::string_view& entry_point
	) const
	{
		std::filesystem::path input_path = m_AssetDir / src_path;
		std::filesystem::path output_path = m_ShaderOutputDir / input_path.filename().replace_extension(".spv");

		if (std::filesystem::exists(output_path) &&
			std::filesystem::last_write_time(output_path)
				> std::filesystem::last_write_time(input_path))
			return ShaderModule(LoadSpv(output_path), stage, entry_point);

		ShaderBinary shader_binary(ShaderString(m_AssetDir / src_path).compile());

		std::ofstream output_file(output_path, std::ios::binary);
		NA_ASSERT(output_file, "Failed to open file {}", output_path.C_STR());

		output_file.write((const char*)shader_binary.ptr(), shader_binary.size());
		output_file.close();

		return ShaderModule(shader_binary, stage, entry_point);
	}

	ShaderModule AssetRegistry::create_shader_module_from_str(
		const std::string_view& name,
		const std::string_view& src,
		ShaderStageBits stage,
		const std::string_view& entry_point
	) const
	{
		std::filesystem::path output_path = (m_ShaderOutputDir / name).replace_extension(".spv");

		ShaderBinary shader_binary(ShaderString(src, name).compile());

		std::ofstream output_file(output_path, std::ios::binary);
		NA_ASSERT(output_file, "Failed to open file {}", output_path.C_STR());

		output_file.write((const char*)shader_binary.ptr(), shader_binary.size());
		output_file.close();

		return ShaderModule(shader_binary, stage, entry_point);
	}
} // namespace Na
