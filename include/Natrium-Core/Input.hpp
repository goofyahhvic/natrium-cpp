#if !defined(NA_INPUT_HPP)
#define NA_INPUT_HPP

#include "InputMacros.h"
#include "./Event.hpp"

namespace Na {
	class Input {
	public:
		using MousePos = glm::vec2;

		Input(void) { this->reset(); }
		~Input(void) = default;

		void on_event(const Event& e);
		inline void reset(void) { memset(this, 0, sizeof(Input)); }

		[[nodiscard]] inline bool  key(u16 index) const { return m_Keys[index]; }
		[[nodiscard]] inline bool* keys(void) { return m_Keys.data(); }

		[[nodiscard]] inline bool  mouse_button(u8 index) const { return m_MBs[index]; }
		[[nodiscard]] inline bool* mouse_buttons(void) { return m_MBs.data(); }

		[[nodiscard]] inline float mouse_x(void) const { return m_MousePos.x; }
		[[nodiscard]] inline float mouse_y(void) const { return m_MousePos.y; }
		[[nodiscard]] inline MousePos& mouse_pos(void) { return m_MousePos; }
	private:
		std::array<bool, NA_KEY_LAST> m_Keys;
		std::array<bool, NA_MOUSE_BUTTON_LAST> m_MBs;
		MousePos m_MousePos;
	};
} // namespace Na

#endif // NA_INPUT_HPP