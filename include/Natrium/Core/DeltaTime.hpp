#if !defined(NA_DELTA_TIME_HPP)
#define NA_DELTA_TIME_HPP

#include "Natrium/Core.hpp"

namespace Na {
	class DeltaTime {
	public:
		inline double calculate(void)
		{
			m_Now = std::chrono::steady_clock::now();
			auto difference = std::chrono::duration_cast<std::chrono::microseconds>(m_Now - m_Last);
			m_Last = m_Now;
			m_DeltaTime = difference.count() / 1.0e+6;
			return m_DeltaTime;
		}
		[[nodiscard]] inline double dt(void) const { return m_DeltaTime; }
		[[nodiscard]] inline double get(void) const { return m_DeltaTime; }
		[[nodiscard]] inline operator double(void) const { return m_DeltaTime; }
	private:
		std::chrono::steady_clock::time_point m_Now, m_Last;
		double m_DeltaTime = 0.0f;
	};
} // namespace

#endif // NA_DELTA_TIME_HPP