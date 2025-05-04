#if !defined(NA_TEXTURE_HPP)
#define NA_TEXTURE_HPP

#include "Natrium/Assets/ImageAsset.hpp"
#include "Natrium/Graphics/DeviceImage.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

namespace Na {
	class Texture {
	public:
		Texture(void) = default;
		Texture(const Image& img, const RendererSettings& renderer_settings);
		void destroy(void);
		inline ~Texture(void) { this->destroy(); }

		Texture(const Texture& other) = delete;
		Texture& operator=(const Texture& other) = delete;

		Texture(Texture&& other);
		Texture& operator=(Texture&& other);

		void bind_to_pipeline(u32 binding, GraphicsPipeline& pipeline) const;

		[[nodiscard]] inline operator bool(void) const { return m_Image; }
	private:
		DeviceImage m_Image;
		vk::ImageView m_ImageView = nullptr;
		vk::Sampler m_Sampler = nullptr;
	};
} // namespace Na

#endif // NA_TEXTURE_HPP