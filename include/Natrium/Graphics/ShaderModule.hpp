#if !defined(NA_SHADER_MODULE_HPP)
#define NA_SHADER_MODULE_HPP

#include "Natrium/Assets/ShaderAsset.hpp"

namespace Na {
	class ShaderModule {
	public:
		inline ShaderModule(
			vk::ShaderModule module = nullptr,
			ShaderStageBits stage = ShaderStageBits::None,
			const std::string_view& entry_point = "main"
		)
		: m_Module(module), m_Stage(stage), m_EntryPoint(entry_point)
		{}

		ShaderModule(
			const ShaderBinary& binary,
			ShaderStageBits stage,
			const std::string_view& entry_point = "main"
		);

		~ShaderModule(void);

		ShaderModule(ShaderModule&& other);
		ShaderModule& operator=(ShaderModule&& other);

		[[nodiscard]] inline vk::PipelineShaderStageCreateInfo pipeline_shader_info(void) const { return vk::PipelineShaderStageCreateInfo({}, (vk::ShaderStageFlagBits)this->m_Stage, this->m_Module, this->m_EntryPoint.data()); }

		[[nodiscard]] inline vk::ShaderModule module(void) const { return m_Module; }
		[[nodiscard]] inline ShaderStageBits stage(void) const { return m_Stage; }

		[[nodiscard]] inline std::string_view& entry_point(void) { return m_EntryPoint; }
		[[nodiscard]] inline const std::string_view& entry_point(void) const { return m_EntryPoint; }

		[[nodiscard]] inline operator bool(void) const { return m_Module; };
	private:
		vk::ShaderModule m_Module;
		ShaderStageBits m_Stage;
		std::string_view m_EntryPoint;
	};
}

#endif // NA_SHADER_MODULE_HPP