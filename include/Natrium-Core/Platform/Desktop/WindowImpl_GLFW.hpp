#if !defined(NA_WINDOW_IMPL_GLFW)
#define NA_WINDOW_IMPL_GLFW

#if defined(NA_PLATFORM_LINUX) || defined(NA_PLATFORM_WINDOWS)

#include "Natrium-Core/Core.hpp"
#include "Natrium-Core/Event.hpp"

struct GLFWwindow;

namespace Na {
	void Window_SetGLFWCallbacks(GLFWwindow* window);
	EventQueue& PollEvents(void);

	class WindowImpl_GLFW {
	public:
		WindowImpl_GLFW(u32 width, u32 height, const std::string_view& title);
		void destroy(void);

		void focus(void);
		[[nodiscard]] inline bool focused(void) const { return m_Focus; }

		[[nodiscard]] inline bool minimized(void) const { return m_Minimized; }

		[[nodiscard]] inline u32 width(void) const { return m_Width; }
		[[nodiscard]] inline u32 height(void) const { return m_Height; }
		void set_size(u32 width, u32 height);

		void set_title(const char* title);
		[[nodiscard]] const char* title(void) const;

		[[nodiscard]] inline GLFWwindow* native(void) { return m_Window; }
		inline operator bool(void) const { return m_Window; }
	private:
		friend void Window_SetGLFWCallbacks(GLFWwindow* window);
	private:
		GLFWwindow* m_Window;
		u32 m_Width, m_Height;
		bool m_Focus = true;
		bool m_Minimized = false;
	};

	using Window = WindowImpl_GLFW;
} // namespace Na

#endif // NA_PLATFORM

#endif // NA_WINDOW_IMPL_GLFW