#include "infPCH.h"
#include "graphics/Bitmap.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "core/Assert.h"

using namespace INF::GFX;

void Bitmap::Load(std::string_view filename)
{
	int width{ -1 }, height{ -1 }, channels{ -1 };
	stbi_info(filename.data(), &width, &height, &channels);
	int requiredCompCount = STBI_rgb_alpha;

	INF_ASSERT(width > 0, "Image couldn't be loaded, make sure filename is correct");
	if (width <= 0)
		return;

	m_width = width;
	m_height = height;

	size_t bitmapSize = width * height * requiredCompCount;
	m_data.resize(bitmapSize);

	uint8_t* pixelData = stbi_load(filename.data(), &width, &height, &channels, requiredCompCount);
	memcpy(m_data.data(), pixelData, bitmapSize);
	stbi_image_free(pixelData);

	m_format = Format::RGBA8_UNORM;
}

uint32_t Bitmap::ComponentCount() const
{
	//for now only 4 components are supported
	return 4;
}
