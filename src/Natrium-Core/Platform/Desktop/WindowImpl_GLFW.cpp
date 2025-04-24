#include "Pch.hpp"
#include "Natrium-Core/Platform/Desktop/WindowImpl_GLFW.hpp"

#if defined(NA_PLATFORM_LINUX) || defined(NA_PLATFORM_WINDOWS)

#include "Natrium-Core/Event.hpp"
#include "Natrium-Core/Context.hpp"

#include <GLFW/glfw3.h>

namespace Na {
	EventQueue& PollEvents(void)
	{
		Context::GetEventQueue().resize(0);
		glfwPollEvents();
		return Context::GetEventQueue();
	}

	Window::WindowImpl_GLFW(u32 width, u32 height, const std::string_view& title)
	: m_Width(width), m_Height(height)
	{
		m_Window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
		NA_ASSERT(m_Window, "Failed to create glfw window!");

		glfwSetWindowUserPointer(m_Window, this);

		Window_SetGLFWCallbacks(m_Window);
	}

	void Window::destroy(void)
	{
		if (!m_Window)
			return;

		glfwDestroyWindow(m_Window);
		m_Window = nullptr;
	}

	void Window::focus(void)
	{
		glfwFocusWindow(m_Window);
		m_Focus = true;
	}

	void Window::set_size(u32 width, u32 height)
	{
		glfwSetWindowSize(m_Window, width, height);
		m_Width = width;
		m_Height = height;
	}

	void Window::set_title(const char* title)
	{
		glfwSetWindowTitle(m_Window, title);
	}

	const char* Window::title(void) const
	{
		return glfwGetWindowTitle(m_Window);
	}

	WindowImpl_GLFW::WindowImpl_GLFW(WindowImpl_GLFW&& other)
	: m_Window(std::exchange(other.m_Window, nullptr)),
	m_Width(std::exchange(other.m_Width, 0)),
	m_Height(std::exchange(other.m_Height, 0)),
	m_Focus(other.m_Focus),
	m_Minimized(other.m_Minimized)
	{}

	WindowImpl_GLFW& WindowImpl_GLFW::operator=(WindowImpl_GLFW&& other)
	{
		glfwDestroyWindow(m_Window);

		m_Window = std::exchange(other.m_Window, nullptr);
		m_Width = std::exchange(other.m_Width, 0);
		m_Height = std::exchange(other.m_Height, 0);
		m_Focus = other.m_Focus;
		m_Minimized = other.m_Minimized;

		return *this;
	}

	void Window_SetGLFWCallbacks(GLFWwindow* _window)
	{
		glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
				Context::GetEventQueue().emplace(Event{
					Event_Type::KeyPressed,
					__window,
					false,
					(uint16_t)key,
					false
				});
				break;
			case GLFW_REPEAT:
				Context::GetEventQueue().emplace(Event{
					Event_Type::KeyPressed,
					__window,
					false,
					(uint16_t)key,
					true
				});
				break;
			case GLFW_RELEASE:
				Context::GetEventQueue().emplace(Event{
					Event_Type::KeyReleased,
					__window,
					false,
					(uint16_t)key
				});
				break;
			}
		});
		glfwSetWindowSizeCallback(_window, [](GLFWwindow* window, int width, int height)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			__window->m_Width = width;
			__window->m_Height = height;
			Context::GetEventQueue().emplace(Event{
				.window_resized = {
					Event_Type::WindowResized,
					__window,
					false,
					(u32)width, (u32)height
				}
			});
		});
		glfwSetWindowCloseCallback(_window, [](GLFWwindow* window)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			Context::GetEventQueue().emplace(Event{
				.window_closed = {
					Event_Type::WindowClosed,
					__window,
					false
				}
			});
		});
		glfwSetWindowFocusCallback(_window, [](GLFWwindow* window, int focus)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			if (focus == GLFW_TRUE)
			{
				__window->m_Focus = true;
				Context::GetEventQueue().emplace(Event{
					.window_focused = {
						Event_Type::WindowFocused,
						__window,
						false
					}
				});
			} else
			{
				__window->m_Focus = false;
				Context::GetEventQueue().emplace(Event{
					.window_lost_focus = {
						Event_Type::WindowLostFocus,
						__window,
						false
					}
				});
			}

		});
		glfwSetWindowIconifyCallback(_window, [](GLFWwindow* window, int iconified)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			if (iconified)
			{
				__window->m_Minimized = true;
				Context::GetEventQueue().emplace(Event{
					.window_minimized = {
						Event_Type::WindowMinimized,
						__window,
						false
					}
				});
			} else
			{
				__window->m_Minimized = false;
				Context::GetEventQueue().emplace(Event{
					.window_restored = {
						Event_Type::WindowRestored,
						__window,
						false
					}
				});
			}
		});
		glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double x, double y)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			Context::GetEventQueue().emplace(Event{
				.mouse_moved = {
					Event_Type::MouseMoved,
					__window,
					false,
					(float)x, (float)y
				}
			});
		});
		glfwSetScrollCallback(_window, [](GLFWwindow* window, double x_offset, double y_offset)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			Context::GetEventQueue().emplace(Event{
				.mouse_scrolled = {
					Event_Type::MouseScrolled,
					__window,
					false,
					(float)x_offset, (float)y_offset
				}
			});
		});
		glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			switch (action)
			{
			case GLFW_PRESS:
				Context::GetEventQueue().emplace(Event{
					.mouse_button_pressed = {
						Event_Type::MouseButtonPressed,
						__window,
						false,
						(u8)button
					}
				});
				break;
			case GLFW_RELEASE:
				Context::GetEventQueue().emplace(Event{
					.mouse_button_released = {
						Event_Type::MouseButtonReleased,
						__window,
						false,
						(u8)button
					}
				});
				break;
			}
		});
	}
} // namespace Na

#endif // NA_PLATFORM
