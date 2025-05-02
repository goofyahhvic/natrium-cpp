#include "Pch.hpp"
#include "Natrium/Core/Input.hpp"

#include "Natrium/Core/Logger.hpp"

namespace Na {
	void Input::on_event(const Event& e)
	{
		switch (e.type)
		{
		case EventType::KeyPressed:
			m_Keys[e.key_pressed.key] = true;
			break;
		case EventType::KeyReleased:
			m_Keys[e.key_released.key] = false;
			break;
		case EventType::MouseButtonPressed:
			m_MBs[e.mouse_button_pressed.button] = true;
			break;
		case EventType::MouseButtonReleased:
			m_MBs[e.mouse_button_released.button] = false;
			break;
		case EventType::MouseMoved:
			m_MousePos.x = e.mouse_moved.x;
			m_MousePos.y = e.mouse_moved.y;
			break;
		}
	}

} // namespace Na
