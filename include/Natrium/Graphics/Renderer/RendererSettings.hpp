#if !defined(NA_RENDERER_SETTINGS_HPP)
#define NA_RENDERER_SETTINGS_HPP

#include "Natrium/Core.hpp"

namespace Na {
	struct RendererSettings {
		u32 max_frames_in_flight;

		bool anisotropy_enabled;
		float max_anisotropy;

		bool msaa_enabled;

		static RendererSettings Default(void);
	};
} // namespace Na

#endif // NA_RENDERER_SETTINGS_HPP