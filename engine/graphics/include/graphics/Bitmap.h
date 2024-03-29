#pragma once
#include "graphicDefines.h"

namespace INF::GFX
{
	class Bitmap
	{
	public:
		Bitmap() = default;
		void Load(std::string_view filename);
		void Create(uint32_t width, uint32_t height, const Color& fill);

		const uint8_t* Data() const { return m_data.data(); }
		size_t Size() const { return m_data.size(); }
		uint32_t Width() const { return m_width; }
		uint32_t Height() const { return m_height; }
		Format GetFormat() const { return m_format; }
		uint32_t ComponentCount() const;
	private:
		Format m_format;
		uint32_t m_width, m_height;
		std::vector<uint8_t> m_data;
	};
}