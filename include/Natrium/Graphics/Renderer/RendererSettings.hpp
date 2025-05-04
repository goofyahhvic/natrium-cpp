#if !defined(NA_RENDERER_SETTINGS_HPP)
#define NA_RENDERER_SETTINGS_HPP

#include "Natrium/Core.hpp"

namespace Na {
	struct RendererSettings {
		u32 max_frames_in_flight = 2;

		bool anisotropy_enabled = true;
		float max_anisotropy;

		bool msaa_enabled = true;

		static RendererSettings Default(void);
	};
} // namespace Na

#endif // NA_RENDERER_SETTINGS_HPP