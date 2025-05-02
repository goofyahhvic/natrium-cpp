#if !defined(NA_IMAGE_ASSET_HPP)
#define NA_IMAGE_ASSET_HPP

#include "Natrium/Assets/Asset.hpp"

namespace Na {
	class ImageAsset : public Asset {
	public:
		ImageAsset(void) = default;
		inline ~ImageAsset(void) override { free(m_Data); }

		static AssetHandle<ImageAsset> Load(const std::filesystem::path& path);

		[[nodiscard]] inline void* data(void) const { return m_Data; }
		[[nodiscard]] inline u64 size(void) const { return m_Size; }

		[[nodiscard]] inline int width(void) const { return m_Width; }
		[[nodiscard]] inline int height(void) const { return m_Height; }

		[[nodiscard]] inline operator bool(void) const override { return m_Data; };
	private:
		void* m_Data;
		u64 m_Size;
		int m_Width, m_Height;
	};
	using Image = ImageAsset;
} // namespace Na

#endif // NA_IMAGE_ASSET_HPP