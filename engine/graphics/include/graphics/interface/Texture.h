#pragma once
#include "graphics/graphicDefines.h"

namespace INF::GFX
{
	enum class TextureDimension
	{
		Texture2D,
	};

	struct TextureDesc
	{
		uint32_t width = 1;
		uint32_t height = 1;
		uint32_t depth = 1;
		uint32_t arraySize = 1;
		uint32_t mipLevels = 1;
		Format format = Format::UNKNOWN;
		TextureDimension dimension = TextureDimension::Texture2D;
		const char* name = nullptr;
	};

	class ITextureView
	{
	public:
		enum class ViewType
		{
			RENDER_TARGET,
			SHADER_RESOURCE,
			DEPTH
		};

		ViewType type;
		Format format;
	};


	class ITexture
	{
	public:
		virtual ~ITexture() = default;
		virtual const TextureDesc& GetDesc() const = 0;

		virtual const ITextureView* GetView(ITextureView::ViewType type) = 0;
	};

	using TextureHandle = std::shared_ptr<ITexture>;
}