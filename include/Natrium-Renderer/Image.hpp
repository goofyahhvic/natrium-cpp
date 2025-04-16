#if !defined(NA_IMAGE_HPP)
#define NA_IMAGE_HPP

#include "Natrium-Core/Core.hpp"

namespace Na {
	struct Image {
		void* data;
		u64 size;
		int width, height;

		inline void destroy(void) { free(data); data = nullptr; }

		static Image Load(const std::filesystem::path& path);
		static Image Copy(const Image& other);

		[[nodiscard]] inline operator bool(void) const { return data; }
	};
} // namespace Na

#endif // NA_IMAGE_HPP