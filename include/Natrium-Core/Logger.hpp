#if !defined(NA_LOGGER_HPP)
#define NA_LOGGER_HPP

#include "./Core.hpp"

#define NA_COLOR_RSET  "\x1B[0m"         // reset color
#define NA_COLOR_FRED  "\x1B[31m"        // red
#define NA_COLOR_BRED  "\x1B[0m\033[41m" // red background
#define NA_COLOR_FGRN  "\x1B[32m"        // green 
#define NA_COLOR_FYEL  "\x1B[33m"        // yellow
#define NA_COLOR_FBLU  "\x1B[34m"        // blue
#define NA_COLOR_FMAG  "\x1B[35m"        // magenta
#define NA_COLOR_FCYN  "\x1B[36m"        // cyan
#define NA_COLOR_FWHT  "\x1B[37m"        // white

#define NA_FONT_BOLD "\x1B[1m"           // bold
#define NA_FONT_UNDL "\x1B[4m"           // underline

namespace Na {
	enum LogLevel : u8 {
		None = 0,
		Trace, Info, Warn, Error, Fatal
	};
	inline std::string_view LogLevelStr(LogLevel level)
	{
		switch (level)
		{
		case Trace: return NA_FONT_BOLD;
		case Info:  return NA_FONT_BOLD NA_COLOR_FGRN;
		case Warn:  return NA_FONT_BOLD NA_COLOR_FYEL;
		case Error: return NA_FONT_BOLD NA_COLOR_FRED;
		case Fatal: return NA_FONT_BOLD NA_COLOR_BRED;
		}
		return "";
	}

#if !defined(NA_CONFIG_DIST)
	template<typename t_Stream = std::ostream, bool t_Enabled = true>
#else
	template<typename t_Stream = std::ostream, bool t_Enabled = false>
#endif 
	class Logger {
	public:
		std::string_view name;
		t_Stream* stream;
	public:
		inline void log(LogLevel level, const std::string_view& msg)
		{
			if (!t_Enabled)
				return;

			*stream << NA_FORMAT(
				"{}[{:%H:%M:%S}][{}]{}: {}\n",
				LogLevelStr(level),
				std::chrono::round<std::chrono::seconds>(std::chrono::system_clock::now()),
				name,
				NA_COLOR_RSET,
				msg
			);
		}

		template<typename... t_Args>
		inline void fmt(LogLevel level, fmt::format_string<t_Args...> str, t_Args&&... __args)
		{
			if (!t_Enabled)
				return;

			*stream << NA_FORMAT(
				"{}[{:%H:%M:%S}][{}]{}: {}\n",
				LogLevelStr(level),
				std::chrono::round<std::chrono::seconds>(std::chrono::system_clock::now()),
				name,
				NA_COLOR_RSET,
				NA_FORMAT(str, std::forward<t_Args>(__args)...)
			);
		}

		inline void header(LogLevel level = LogLevel::Info)
		{
			if (!t_Enabled)
				return;

			*stream << NA_FORMAT(
				"{}[{:%Y-%m-%d %H:%M:%S}][{}]{}\n",
				LogLevelStr(level),
				std::chrono::round<std::chrono::seconds>(std::chrono::system_clock::now()),
			#if __cpp_lib_chrono >= 201907L
				std::chrono::current_zone()->name(),
			#else
				""
			#endif
				NA_COLOR_RSET
			);
		}

		inline void new_line(void)
		{
			if (!t_Enabled)
				return;

			*stream << '\n';
		}

		inline void operator()(LogLevel level, const std::string_view& msg)
		{
			if (!t_Enabled)
				return;

			this->log(level, msg);
		}

		[[nodiscard]] inline constexpr bool enabled(void) { return t_Enabled; }
	};
	inline Logger<> g_Logger{"Natrium", &std::clog};
} // namespace Na

#endif // NA_LOGGER_HPP