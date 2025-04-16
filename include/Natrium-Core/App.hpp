#if !defined(NEO_APP_HPP)
#define NEO_APP_HPP

#include "./Event.hpp"

namespace Na {
	enum class App_SystemType : u8 {
		None = 0, Init, Shutdown, Update, OnEvent
	};

	struct App_Systems {
		ArrayList<void(*)(void)> init;
		ArrayList<void(*)(void)> shutdown;

		ArrayList<void(*)(double dt)> update;
		ArrayList<void(*)(Event& e)> on_event;
	};
	
	class App {
	public:
		inline App(void) = default;
		inline ~App(void) noexcept(false) = default;
		void run(void);

		void add_system(void* fn, App_SystemType type);
	public:
		bool should_close = false;
		App_Systems subsystems;
	};
} // namespace Neo

#endif // NEO_APP_HPP