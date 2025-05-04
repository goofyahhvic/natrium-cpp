#include "Pch.hpp"
#include "Natrium/Graphics/Renderer/RendererSettings.hpp"

#include "Natrium/Graphics/VkContext.hpp"

namespace Na {
	RendererSettings RendererSettings::Default(void)
	{
		return RendererSettings{
			.max_frames_in_flight = 2,
			.anisotropy_enabled = true,
			.max_anisotropy = VkContext::GetPhysicalDevice().getProperties().limits.maxSamplerAnisotropy,
			.msaa_enabled = true
		};
	}
} // namespace Na
