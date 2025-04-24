#if !defined(NA_APP_HPP)
#define NA_APP_HPP

#include "./Event.hpp"

namespace Na {
	class App {
	public:
		bool should_close = false;
	public:
		App(void) = default;
		virtual ~App(void) = default;

		void run(void);

		inline virtual void on_event(Event& e) { if (e.type == Event_Type::WindowClosed) this->should_close = true; }
		inline virtual void update(double dt) {}
		inline virtual void draw(void) {}
	};
} // namespace Neo

#endif // NA_APP_HPP