#if !defined(NA_SHADER_HPP)
#define NA_SHADER_HPP

#include "Natrium-Core/Core.hpp"

namespace Na {
	enum class ShaderStageBits : u32 {
		None = 0,
		Vertex = vk::ShaderStageFlagBits::eVertex,
		Fragment = vk::ShaderStageFlagBits::eFragment,


		Pixel = Fragment,
		All = vk::ShaderStageFlagBits::eAll
	};

	class Shader {
	public:
		Shader(void) : m_Stage(ShaderStageBits::None) {}
		Shader(vk::ShaderModule module, ShaderStageBits stage, const std::string_view& entry_point = "main"sv) : m_Module(module), m_Stage(stage), m_EntryPoint(entry_point) {}

		Shader(const std::filesystem::path& path, ShaderStageBits stage, const std::string_view& entry_point = "main"sv);
		void destroy(void);

		[[nodiscard]] inline vk::PipelineShaderStageCreateInfo pipeline_shader_info(void) { return vk::PipelineShaderStageCreateInfo({}, (vk::ShaderStageFlagBits)m_Stage, m_Module, m_EntryPoint.data()); }

		[[nodiscard]] inline const std::string_view& entry_point(void) const { return m_EntryPoint; }
		[[nodiscard]] inline ShaderStageBits stage(void) const { return m_Stage; }
		[[nodiscard]] inline vk::ShaderModule module(void) const { return m_Module; }
	private:
		vk::ShaderModule m_Module;
		ShaderStageBits m_Stage;
		std::string_view m_EntryPoint;
	};
} // namespace Na

#endif // NA_SHADER_HPP