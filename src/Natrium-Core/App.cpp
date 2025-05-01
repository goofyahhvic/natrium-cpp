#include "Pch.hpp"
#include "Natrium-Core/App.hpp"

#include "Natrium-Renderer/VkContext.hpp"
#include "Natrium-Core/Window.hpp"

namespace Na {
	void App::run(void)
	{
		std::chrono::steady_clock::time_point now, last;

		while (!should_close)
		{
			now = std::chrono::steady_clock::now();
			auto difference = std::chrono::duration_cast<std::chrono::microseconds>(now - last);
			last = now;

			double dt = difference.count() / 1e+6;
			this->update(dt);

			this->draw();

			for (Event& e : PollEvents())
				this->on_event(e);
		}
		VkContext::GetLogicalDevice().waitIdle();
	}
} // namespace Na
