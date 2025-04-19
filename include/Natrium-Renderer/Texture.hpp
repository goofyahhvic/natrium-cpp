#if !defined(NA_TEXTURE_HPP)
#define NA_TEXTURE_HPP

#include "./Image.hpp"
#include "./DeviceImage.hpp"
#include "./Renderer.hpp"

namespace Na {
	class Texture {
	public:
		Texture(const Image& img, u32 binding, Renderer& renderer);
		void destroy(void);
	private:
		DeviceImage m_Image;
		vk::ImageView m_ImageView;
		vk::Sampler m_Sampler;
	};
} // namespace Na

#endif // NA_TEXTURE_HPP