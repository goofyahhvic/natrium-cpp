#if !defined(NA_TEXTURE_HPP)
#define NA_TEXTURE_HPP

#include "Natrium/Assets/ImageAsset.hpp"
#include "Natrium/Graphics/DeviceImage.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

namespace Na {
	class Texture {
	public:
		const ShaderUniformType descriptor_type = ShaderUniformType::Texture;

		Texture(void) = default;
		Texture(const Image& img, const RendererSettings& renderer_settings);

		inline ~Texture(void) { this->destroy(); }
		void destroy(void);

		Texture(const Texture& other) = delete;
		Texture& operator=(const Texture& other) = delete;

		Texture(Texture&& other);
		Texture& operator=(Texture&& other);

		[[nodiscard]] inline operator bool(void) const { return m_Image; }

		[[nodiscard]] inline const DeviceImage& img(void) const { return m_Image; }
		[[nodiscard]] inline vk::ImageView img_view(void) const { return m_ImageView; }
		[[nodiscard]] inline vk::Sampler sampler(void) const { return m_Sampler; }
	private:
		DeviceImage m_Image;
		vk::ImageView m_ImageView = nullptr;
		vk::Sampler m_Sampler = nullptr;
	};
} // namespace Na

#endif // NA_TEXTURE_HPP