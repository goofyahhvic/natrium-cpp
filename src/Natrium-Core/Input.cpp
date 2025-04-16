#include "Pch.hpp"
#include "Natrium-Core/Input.hpp"

namespace Na {
	void Input::on_event(const Event& e)
	{
		switch (e.type)
		{
		case Event_Type::KeyPressed:
			m_Keys[e.key_pressed.key] = true;
			break;
		case Event_Type::KeyReleased:
			m_Keys[e.key_released.key] = false;
			break;
		case Event_Type::MouseButtonPressed:
			m_MBs[e.mouse_button_pressed.button] = true;
			break;
		case Event_Type::MouseButtonReleased:
			m_MBs[e.mouse_button_released.button] = false;
			break;
		case Event_Type::MouseMoved:
			m_MousePos.x = e.mouse_moved.x;
			m_MousePos.y = e.mouse_moved.y;
			break;
		}
	}

} // namespace Na
