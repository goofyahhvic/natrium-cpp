#if !defined(NA_INPUT_HPP)
#define NA_INPUT_HPP

#include "Natrium/Core/InputConstants.hpp"
#include "Natrium/Core/Event.hpp"

namespace Na {
	class Input {
	public:
		using MousePos = glm::vec2;

		Input(void) { this->reset(); }
		~Input(void) = default;

		void reset(void);
		void on_event(const Event& e);

		[[nodiscard]] inline bool key(Key key) const { return m_Keys.test(key); }
		[[nodiscard]] inline bool mouse_button(MouseButton button) const { return m_MouseButtons.test(button); }

		[[nodiscard]] inline float mouse_x(void) const { return m_MousePos.x; }
		[[nodiscard]] inline float mouse_y(void) const { return m_MousePos.y; }

		[[nodiscard]] inline bool gamepad_connected(Joystick jid) const { return m_Gamepads.test(jid); }
		[[nodiscard]] inline bool gamepad_button(Joystick jid, GamepadButton button) const { return m_GamepadButtons.test(this->_GamepadButtonIndex(jid, button)); }
		[[nodiscard]] inline float gamepad_axis(Joystick jid, GamepadAxis axis) const { return m_GamepadAxes[this->_GamepadAxisIndex(jid, axis)]; }
	private:
		[[nodiscard]] static constexpr inline u64 _GamepadButtonIndex(Joystick jid, GamepadButton button) { return (u64)jid + (u64)button * ((u64)GamepadButtons::k_Last + 1); }
		[[nodiscard]] static constexpr inline u64 _GamepadAxisIndex(Joystick jid, GamepadAxis axis) { return (u64)jid + (u64)axis * ((u64)GamepadAxes::k_Last + 1); }
	private:
		std::bitset<Keys::k_Last + 1> m_Keys;

		std::bitset<MouseButtons::k_Last + 1> m_MouseButtons;
		MousePos m_MousePos;

		std::bitset<Joysticks::k_Last + 1> m_Gamepads;
		std::bitset<(GamepadButtons::k_Last + 1) * Joysticks::k_Last> m_GamepadButtons;
		std::array<float, (GamepadAxes::k_Last + 1) * Joysticks::k_Last> m_GamepadAxes;
	};
} // namespace Na

#endif // NA_INPUT_HPP