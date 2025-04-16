#include "Pch.hpp"
#include "Natrium-Renderer/Image.hpp"

#include <stb/stb_image.h>

namespace Na {
	Image Image::Load(const std::filesystem::path& path)
	{
		Image img;

		int channels;
		img.data = (void*)stbi_load(
			path.string().c_str(),
			&img.width,
			&img.height,
			&channels,
			STBI_rgb_alpha
		);
		img.size = (u64)img.width * img.height * 4;

		return img;
	}

	Image Image::Copy(const Image& other)
	{
		Image img;

		img.data = malloc(other.size);
		memcpy(img.data, other.data, other.size);

		img.size = other.size;

		img.width = other.width;
		img.height = other.height;

		return img;
	}

} // namespace Na
