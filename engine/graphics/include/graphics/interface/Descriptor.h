#pragma once
#include "graphics/graphicDefines.h"
#include <cstdint>
#include <array>
#include <memory>

namespace INF::GFX
{
	class ITexture;
	class IBuffer;

	static constexpr uint32_t MaxBindingsPerStage = 32;

	enum class ResourceType : uint8_t
	{
		TEXTURE_SRV,
		BUFFER_SRV,
		CONSTANTBUFFER,
		SAMPLER,
	};

	struct DescriptorLayoutItem
	{
		uint32_t slot = UINT_MAX;
		ResourceType type = ResourceType::TEXTURE_SRV;
		uint32_t registerSpace = 0;

		bool operator ==(const DescriptorLayoutItem& b) const { return slot == b.slot && type == b.type; }
		bool operator !=(const DescriptorLayoutItem& b) const { return !(*this == b); }
	};
	using StageBindingDescriptorDesc = std::array<DescriptorLayoutItem, MaxBindingsPerStage>;

	struct DescriptorLayoutDesc
	{
		StageBindingDescriptorDesc VS;
		StageBindingDescriptorDesc PS;

		StageBindingDescriptorDesc ALL;
	};

	//Describes the layout of descriptors (creates a root signature for d3d12), this is only the layout and doesn't contain the actual binding to the data (ITexture,IBuffer)
	class IDescriptorLayout
	{
	public:
		virtual const DescriptorLayoutDesc& GetDesc() const = 0;
	};
	using DescriptorLayoutHandle = std::shared_ptr<IDescriptorLayout>;


	//The DescriptorSetItem is what actually binds a resource to the pipeline
	struct DescriptorSetItem
	{
		uint32_t slot;
		ResourceType type;
		uint32_t registerSpace;
		Format format;

		union
		{
			ITexture* texture;
			IBuffer* buffer;
		} resourceHandle;
	};
	using StageDescriptorSetDesc = std::array<DescriptorSetItem, MaxBindingsPerStage>;
}