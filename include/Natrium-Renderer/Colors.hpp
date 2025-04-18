#if !defined(NA_COLORS_HPP)
#define NA_COLORS_HPP

namespace Na {
	namespace Colors {
		constexpr glm::vec4 k_Black  = { 0.0f, 0.0f, 0.0f, 1.0f };
		constexpr glm::vec4 k_White  = { 1.0f, 1.0f, 1.0f, 1.0f };

		constexpr glm::vec4 k_Red    = { 1.0f, 0.0f, 0.0f, 1.0f };
		constexpr glm::vec4 k_Green  = { 0.0f, 1.0f, 0.0f, 1.0f };
		constexpr glm::vec4 k_Blue   = { 0.0f, 0.0f, 1.0f, 1.0f };

		constexpr glm::vec4 k_Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
		constexpr glm::vec4 k_Purple = { 1.0f, 0.0f, 1.0f, 1.0f };
		constexpr glm::vec4 k_Cyan   = { 0.0f, 1.0f, 1.0f, 1.0f };
	} // namespace Colors
} // namespace Na

#endif // NA_COLORS_HPP