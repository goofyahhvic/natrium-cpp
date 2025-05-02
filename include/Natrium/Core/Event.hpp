#if !defined(NA_EVENT_HPP)
#define NA_EVENT_HPP

#include "Natrium/Core.hpp"
#include "Natrium/Template/ArrayList.hpp"

#define NA_EVENT_BASE(x) struct {\
						Na::Event_Type type = Na::Event_Type::x;\
						Na::Window* window;\
						bool handled = false;\
					 }

namespace Na {
#if defined(NA_PLATFORM_LINUX) || defined(NA_PLATFORM_WINDOWS)
	class WindowImpl_GLFW;
	using Window = WindowImpl_GLFW;
#endif // NA_PLATFORM

	enum class Event_Type : uint8_t {
		None = 0,
		KeyPressed, KeyReleased,
		WindowResized, WindowClosed, WindowFocused, WindowLostFocus, WindowMinimized, WindowRestored,
		MouseMoved, MouseScrolled, MouseButtonPressed, MouseButtonReleased
	};

	struct Event_KeyPressed {
		NA_EVENT_BASE(KeyPressed);
		uint16_t key;
		bool repeat;
		uint32_t padding = 0;
	};

	struct Event_KeyReleased {
		NA_EVENT_BASE(KeyReleased);
		uint16_t key;
		uint32_t padding = 0;
	};

	struct Event_WindowResized {
		NA_EVENT_BASE(WindowResized);
		uint32_t width, height;
	};

	struct Event_WindowClosed {
		NA_EVENT_BASE(WindowClosed);
		uint32_t padding1 = 0, padding2 = 0;
	};

	struct Event_WindowFocused {
		NA_EVENT_BASE(WindowFocused);
		uint32_t padding1 = 0, padding2 = 0;
	};
	struct Event_WindowLostFocus {
		NA_EVENT_BASE(WindowLostFocus);
		uint32_t padding1 = 0, padding2 = 0;
	};

	struct Event_WindowMinimized {
		NA_EVENT_BASE(WindowMinimized);
		uint32_t padding1 = 0, padding2 = 0;
	};
	struct Event_WindowRestored {
		NA_EVENT_BASE(WindowRestored);
		uint32_t padding1 = 0, padding2 = 0;
	};

	struct Event_MouseMoved {
		NA_EVENT_BASE(MouseMoved);
		float x, y;
	};

	struct Event_MouseScrolled {
		NA_EVENT_BASE(MouseScrolled);
		float x_offset, y_offset;
	};

	struct Event_MouseButtonPressed {
		NA_EVENT_BASE(MouseButtonPressed);
		uint8_t button;
		uint32_t padding = 0;
	};
	struct Event_MouseButtonReleased {
		NA_EVENT_BASE(MouseButtonReleased);
		uint8_t button;
		uint32_t padding = 0;
	};

	union Event {
		struct {
			Event_Type type;
			Window* window;
			bool handled;
			uint32_t padding1, padding2;
		};

		Event_KeyPressed key_pressed;
		Event_KeyReleased key_released;

		Event_WindowResized window_resized;
		Event_WindowClosed window_closed;

		Event_WindowFocused window_focused;
		Event_WindowLostFocus window_lost_focus;

		Event_WindowMinimized window_minimized;
		Event_WindowRestored window_restored;

		Event_MouseMoved mouse_moved;
		Event_MouseScrolled mouse_scrolled;

		Event_MouseButtonPressed mouse_button_pressed;
		Event_MouseButtonReleased mouse_button_released;
	};

	using EventQueue = ArrayList<Event>;
} // namespace Na

#endif // NA_EVENT_HPP