#if !defined(NA_TEXTURE_HPP)
#define NA_TEXTURE_HPP

#include "./Image.hpp"
#include "./DeviceImage.hpp"
#include "./Renderer.hpp"

namespace Na {
	class Texture {
	public:
		Texture(void) = default;
		Texture(const Image& img, u32 binding, Renderer& renderer);
		void destroy(void);
		inline ~Texture(void) { this->destroy(); }

		Texture(const Texture& other) = delete;
		Texture& operator=(const Texture& other) = delete;

		Texture(Texture&& other);
		Texture& operator=(Texture&& other);

		[[nodiscard]] inline operator bool(void) const { return m_Image; }
	private:
		DeviceImage m_Image;
		vk::ImageView m_ImageView = nullptr;
		vk::Sampler m_Sampler = nullptr;
	};
} // namespace Na

#endif // NA_TEXTURE_HPP