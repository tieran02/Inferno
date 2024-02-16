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
		TRANSITION_STATES_FLAGS initialState = (TRANSITION_STATES_FLAGS)TRANSITION_STATES::COMMON;
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

	struct SamplerDesc
	{


		WrapMode wrapMode[3];
		float mipBias, anisotropy;
		bool minFilter, magFilter, mipFilter;
		ReductionType reductionType;
		Color borderColor;

		SamplerDesc() :
			minFilter(true),
			magFilter(true),
			mipFilter(true),
			mipBias(0),
			anisotropy(1),
			reductionType(ReductionType::STANDARD),
			borderColor(1, 1, 1, 1)
		{
			wrapMode[0] = wrapMode[1] = wrapMode[2] = WrapMode::CLAMP;
		}
	};

	class ISampler
	{
	public:
		virtual ~ISampler() = default;
		virtual const SamplerDesc& GetDesc() const = 0;
	};
	using SamplerHandle = std::shared_ptr<ISampler>;

	struct FramebufferAttachment
	{
		ITexture* texture;

		FramebufferAttachment(ITexture* texture = nullptr) : texture(texture) {}
	};

	struct FramebufferDesc
	{
		std::array<FramebufferAttachment, MAX_RENDER_TARGETS> colorAttachments;
		FramebufferAttachment depthAttachment;
	};

	//Only contains formats for each target and the width/height
	struct FramebufferInfo
	{
		std::array<Format, MAX_RENDER_TARGETS> colorFormats;
		Format depthFormat = Format::UNKNOWN;
		uint32_t width = 0;
		uint32_t height = 0;

		FramebufferInfo()
		{ }

		FramebufferInfo(const FramebufferDesc& desc)
		{
			if (desc.depthAttachment.texture != nullptr)
			{
				const TextureDesc& textureDesc = desc.depthAttachment.texture->GetDesc();
				depthFormat = textureDesc.format;
				width = textureDesc.width;
				height = textureDesc.height;
			}

			for (size_t i = 0; i < desc.colorAttachments.size(); i++)
			{
				const FramebufferAttachment& attachment = desc.colorAttachments[i];
				colorFormats[i] = (attachment.texture ? attachment.texture->GetDesc().format : Format::UNKNOWN);

				if (attachment.texture && width == 0)
					width = attachment.texture->GetDesc().width;
				if (attachment.texture && height == 0)
					height = attachment.texture->GetDesc().height;
			}
		}
	};

	//Frame buffer is a vulkan conception and will hold the renderpass and framebuffers
	//D3D12 doesn't have the concept of a framebuffer but we still use it to group the target
	class IFramebuffer
	{
	public:
		virtual const FramebufferDesc& GetDesc() const = 0;
		virtual const FramebufferInfo& GetInfo() const = 0;
	};
	using FramebufferHandle = std::shared_ptr<IFramebuffer>;
}