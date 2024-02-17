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
	using StageDescriptorSetDesc = std::array<DescriptorSetItem, MaxBindingsPerStage>;

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
}


namespace std {
	template <>
	struct hash<INF::GFX::DescriptorLayoutItem> {
		auto operator()(const INF::GFX::DescriptorLayoutItem& item) const -> size_t
		{
			using std::size_t;
			using std::hash;

			return ((hash<uint32_t>()(item.slot)
				^ (hash<uint8_t>()((uint8_t)item.type) << 1)) >> 1);
		}
	};
}  // namespace std