#if !defined(NA_SHADER_HPP)
#define NA_SHADER_HPP

#include "Natrium-Core/Core.hpp"

namespace Na {
	enum class ShaderStageBits : u32 {
		None = 0,
		Vertex   = (u32)vk::ShaderStageFlagBits::eVertex,
		Fragment = (u32)vk::ShaderStageFlagBits::eFragment,


		Pixel    = Fragment,
		All      = (u32)vk::ShaderStageFlagBits::eAll
	};

	class Shader {
	public:
		vk::ShaderModule module;
		ShaderStageBits stage = ShaderStageBits::None;
		std::string_view entry_point;

		Shader(const std::filesystem::path& path, ShaderStageBits stage, const std::string_view& entry_point = "main"sv);
		void destroy(void);

		[[nodiscard]] inline vk::PipelineShaderStageCreateInfo pipeline_shader_info(void) const { return vk::PipelineShaderStageCreateInfo({}, (vk::ShaderStageFlagBits)this->stage, this->module, this->entry_point.data()); }

		[[nodiscard]] inline operator bool(void) const { return this->module; }
	};
} // namespace Na

#endif // NA_SHADER_HPP