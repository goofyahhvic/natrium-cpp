#include "Pch.hpp"
#include "Natrium-Core/App.hpp"

#include "Natrium-Core/Window.hpp"

namespace Na {
	void App::add_system(void* fn, App_SystemType type)
	{
		switch (type)
		{
		case App_SystemType::Init:
			subsystems.init.emplace((void (*)(void))fn);
			break;
		case App_SystemType::Shutdown:
			subsystems.shutdown.emplace((void (*)(void))fn);
			break;
		case App_SystemType::Update:
			subsystems.update.emplace((void (*)(double))fn);
			break;
		case App_SystemType::OnEvent:
			subsystems.on_event.emplace((void (*)(Event&))fn);
			break;
		}
	}

	void App::run(void)
	{
		for (auto init_fn : subsystems.init)
			init_fn();

		std::chrono::steady_clock::time_point now, last;

		while (!should_close)
		{
			now = std::chrono::steady_clock::now();
			auto difference = std::chrono::duration_cast<std::chrono::microseconds>(now - last);
			last = now;

			double dt = difference.count() / 1e+6;
			for (auto update_fn : subsystems.update)
				update_fn(dt);

			for (Event& e : PollEvents())
			{
				for (auto on_event_fn : subsystems.on_event)
				{
					on_event_fn(e);
					if (e.handled)
						break;
				}
			}
		}

		for (auto shutdown_fn : subsystems.shutdown)
			shutdown_fn();
	}
} // namespace Neo
