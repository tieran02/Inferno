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


void Bitmap::Create(uint32_t width, uint32_t height, const Color& fill)
{
	m_width = width;
	m_height = height;

	size_t bitmapSize = width * height * 4;
	m_data.resize(bitmapSize);

	uint8_t color[4];
	color[0] = (uint8_t)std::clamp((fill.R * 256.0f), 0.0f, 255.0f);
	color[1] = (uint8_t)std::clamp((fill.G * 256.0f), 0.0f, 255.0f);
	color[2] = (uint8_t)std::clamp((fill.B * 256.0f), 0.0f, 255.0f);
	color[3] = (uint8_t)std::clamp((fill.A * 256.0f), 0.0f, 255.0f);

	uint8_t* dest = (uint8_t*)m_data.data();
	std::fill_n(dest, bitmapSize, *color);
}


uint32_t Bitmap::ComponentCount() const
{
	//for now only 4 components are supported
	return 4;
}
