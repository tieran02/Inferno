#pragma once
#include "graphics/graphicDefines.h"
#include <cstdint>
#include <array>
#include <memory>

namespace INF::GFX
{
	class ITexture;
	class IBuffer;
	class ISampler;

	static constexpr uint32_t MaxDescriptorLayouts = 5;
	static constexpr uint32_t MaxDescriptorsPerStage = 32;


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

		//size_t Hash() const { std::hash<DescriptorLayoutItem>()(*this); }
	};
	using StageBindingDescriptorDesc = std::array<DescriptorLayoutItem, MaxDescriptorsPerStage>;

	struct DescriptorLayoutDesc
	{
		StageBindingDescriptorDesc VS;
		StageBindingDescriptorDesc PS;

		StageBindingDescriptorDesc ALL;

		//size_t Hash() const { std::hash<DescriptorLayoutDesc>()(*this); }
	};

	//Describes the layout of descriptors (creates a root signature for d3d12), this is only the layout and doesn't contain the actual binding to the data (ITexture,IBuffer)
	class IDescriptorLayout
	{
	public:
		virtual const DescriptorLayoutDesc& GetDesc() const = 0;
	};
	using DescriptorLayoutHandle = std::shared_ptr<IDescriptorLayout>;
	using DescriptorLayoutGroup = std::array<DescriptorLayoutHandle, MaxDescriptorLayouts>;


	union ResourceHandle
	{
		ITexture* texture;
		IBuffer* buffer;
		ISampler* sampler;

		ResourceHandle() : texture(nullptr) {}
		ResourceHandle(ITexture* _texture) : texture(_texture) {}
		ResourceHandle(IBuffer* _buffer) : buffer(_buffer) {}
		ResourceHandle(ISampler* _sampler) : sampler(_sampler) {}
	};

	//The DescriptorSetItem is what actually binds a resource to the pipeline
	struct DescriptorSetItem
	{
		uint32_t slot = UINT_MAX;
		ResourceType type = ResourceType::TEXTURE_SRV;;
		uint32_t registerSpace = 0;
		Format format = Format::UNKNOWN;
		ResourceHandle resourceHandle;

		static inline DescriptorSetItem SRV(uint32_t slot, ITexture* texture, Format format = Format::UNKNOWN, uint32_t registerSpace = 0)
		{
			return DescriptorSetItem
			{
				.slot = slot,
				.type = ResourceType::TEXTURE_SRV,
				.registerSpace = registerSpace,
				.format = format,
				.resourceHandle = ResourceHandle(texture),
			};
		};

		static inline DescriptorSetItem SRV(uint32_t slot, IBuffer* buffer, Format format = Format::UNKNOWN, uint32_t registerSpace = 0)
		{
			return DescriptorSetItem
			{
				.slot = slot,
				.type = ResourceType::BUFFER_SRV,
				.registerSpace = registerSpace,
				.format = format,
				.resourceHandle = ResourceHandle(buffer),
			};
		};

		static inline DescriptorSetItem ConstantBuffer(uint32_t slot, IBuffer* buffer, uint32_t registerSpace = 0)
		{
			return DescriptorSetItem
			{
				.slot = slot,
				.type = ResourceType::CONSTANTBUFFER,
				.registerSpace = registerSpace,
				.format = Format::UNKNOWN,
				.resourceHandle = ResourceHandle(buffer),
			};
		};

		static inline DescriptorSetItem Sampler(uint32_t slot, ISampler* sampler, uint32_t registerSpace = 0)
		{
			return DescriptorSetItem
			{
				.slot = slot,
				.type = ResourceType::SAMPLER,
				.registerSpace = registerSpace,
				.format = Format::UNKNOWN,
				.resourceHandle = ResourceHandle(sampler),
			};
		};
	};
	using StageDescriptorSetDesc = std::array<DescriptorSetItem, MaxDescriptorsPerStage>;

	struct DescriptorSetDesc
	{
		StageDescriptorSetDesc VS;
		StageDescriptorSetDesc PS;
		StageDescriptorSetDesc ALL;
	};

	class IDescriptorSet
	{
	public:
		virtual const DescriptorSetDesc& GetDesc() const = 0;
		virtual IDescriptorLayout* GetLayout() const = 0;
	};
	using DescriptorSetHandle = std::shared_ptr<IDescriptorSet>;
	using DescriptorSetGroup = std::array<DescriptorSetHandle, MaxDescriptorLayouts>;

}


namespace std {
	template <>
	struct hash<INF::GFX::DescriptorLayoutItem> 
	{
		auto operator()(const INF::GFX::DescriptorLayoutItem& item) const -> size_t
		{
			using std::size_t;
			using std::hash;

			return ((hash<uint32_t>()(item.slot)
				^ (hash<uint8_t>()((uint8_t)item.type) << 1)) >> 1);
		}
	};

	template <>
	struct hash<INF::GFX::DescriptorLayoutDesc>
	{
		auto operator()(const INF::GFX::DescriptorLayoutDesc& item) const -> size_t
		{
			using std::size_t;
			using std::hash;

			std::size_t seed = INF::GFX::MaxDescriptorsPerStage;
			for (const INF::GFX::DescriptorLayoutItem& i : item.VS)
			{
				seed ^= hash<INF::GFX::DescriptorLayoutItem>()(i);
			}
			for (const INF::GFX::DescriptorLayoutItem& i : item.PS)
			{
				seed ^= hash<INF::GFX::DescriptorLayoutItem>()(i);
			}
			for (const INF::GFX::DescriptorLayoutItem& i : item.ALL)
			{
				seed ^= hash<INF::GFX::DescriptorLayoutItem>()(i);
			}
			return seed;
		}
	};

	template <>
	struct hash<INF::GFX::DescriptorLayoutGroup>
	{
		auto operator()(const INF::GFX::DescriptorLayoutGroup& set) const -> size_t
		{
			using std::size_t;
			using std::hash;

			std::size_t seed = INF::GFX::MaxDescriptorLayouts;
			for (const INF::GFX::DescriptorLayoutHandle& i : set)
			{
				if (i)
				{
					seed ^= hash<INF::GFX::DescriptorLayoutDesc>()(i->GetDesc());
				}
				else
				{
					seed ^= 0x3000005;
				}
			}
			return seed;
		}
	};
}  // namespace std