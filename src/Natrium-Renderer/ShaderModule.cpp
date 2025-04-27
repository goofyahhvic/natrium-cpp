#include "Pch.hpp"
#include "Natrium-Renderer/ShaderModule.hpp"

#include "Natrium-Renderer/VkContext.hpp"

namespace Na {
	ShaderModule::ShaderModule(
		const ShaderBinary& binary,
		ShaderStageBits stage,
		const std::string_view& entry_point
	)
	: m_Module(VkContext::GetLogicalDevice()
			   .createShaderModule(vk::ShaderModuleCreateInfo(
				   {},
				   binary.size(),
				   binary.ptr()
			   ))),
	m_Stage(stage),
	m_EntryPoint(entry_point)
	{}

	ShaderModule::~ShaderModule(void)
	{
		VkContext::GetLogicalDevice().destroyShaderModule(m_Module);
	}

	ShaderModule::ShaderModule(ShaderModule&& other)
	: m_Module(std::exchange(other.m_Module, nullptr)),
	m_Stage(std::move(other.m_Stage)),
	m_EntryPoint(std::move(other.m_EntryPoint))
	{}

	ShaderModule& ShaderModule::operator=(ShaderModule&& other)
	{
		VkContext::GetLogicalDevice().destroyShaderModule(m_Module);
		m_Module = std::exchange(other.m_Module, nullptr);
		m_Stage = std::move(other.m_Stage);
		m_EntryPoint = std::move(other.m_EntryPoint);
		return *this;
	}
} // namespace Na
