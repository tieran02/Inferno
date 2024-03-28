#pragma once
#include "graphicDefines.h"
#include "interface/Texture.h"
#include "core/Assert.h"
#include "interface/Descriptor.h"
#include "interface/Buffer.h"

namespace INF::GFX
{
	struct MaterialData
	{
		glm::vec4 diffuseColour = glm::vec4(1, 1, 1, 1);
	};

	constexpr uint8_t MAX_TEXTURE_COUNT_PER_MATERIAL = 8;
	class IDevice;

	class Material
	{
	public:
		Material(std::string_view name);

		template <typename T>
		void SetObject(IDevice* device)
		{
			size_t size = sizeof(T);
			CreateBuffer(device, size);

			T defualt;
			memcpy((void*)m_constantBufferDataPtr, &defualt, size);
		}

		template<typename T, typename U>
		void RegisterData(std::string_view name, U T::* member)
		{
			size_t offset = (char*)&((T*)nullptr->*member) - (char*)nullptr;
			m_constantDataOffsetMap.emplace(name, offset);
		}
		void RegisterTexture(std::string_view name, uint8_t textureSlot);

		void SetData(std::string_view name, const float& data);
		void SetData(std::string_view name, const uint32_t& data);
		void SetData(std::string_view name, const glm::vec2& data);
		void SetData(std::string_view name, const glm::vec3& data);
		void SetData(std::string_view name, const glm::vec4& data);
		void SetData(std::string_view name, const glm::mat3& data);
		void SetData(std::string_view name, const glm::mat4& data);
		void SetData(std::string_view name, const TextureHandle& texture, const SamplerHandle& sampler);

		void UpdateDescriptorSet(IDevice* device);

		template <typename T>
		const T& As() const { return *reinterpret_cast<const T*>(m_constantBufferDataPtr); }
		const uint8_t* Data() const { m_constantBufferDataPtr; }
		size_t Size() const { m_constantBufferSize; }

		const std::array<TextureHandle, MAX_TEXTURE_COUNT_PER_MATERIAL>& Textures() const { return m_textures; }
		const std::array<SamplerHandle, MAX_TEXTURE_COUNT_PER_MATERIAL>& Samplers() const { return m_samplers; }

		static DescriptorLayoutDesc DescriptorLayout();
		const DescriptorSetHandle& GetDescriptorSet() const { return m_descriptorHandle; }

	private:
		template<typename U>
		void SetDataT(std::string_view name, const U& data)
		{
			auto it = m_constantDataOffsetMap.find(std::string(name));
			INF_ASSERT(it != m_constantDataOffsetMap.end(), std::format("No data exists with name:{0}", name));
			U* dest = (U*)(m_constantBufferDataPtr + it->second);
			*dest = data;
		}

		void CreateBuffer(IDevice* device,size_t size);

		//This is a 1-1 match of the HLSL struct in the shaders
		//std::vector<uint8_t> m_constantBufferData;
		uint8_t* m_constantBufferDataPtr;
		size_t m_constantBufferSize;

		std::string m_name;

		std::unordered_map<std::string, size_t> m_constantDataOffsetMap;
		std::unordered_map<std::string, uint8_t> m_textureSlotMap;
		std::array<TextureHandle, MAX_TEXTURE_COUNT_PER_MATERIAL> m_textures;
		std::array<SamplerHandle, MAX_TEXTURE_COUNT_PER_MATERIAL> m_samplers;

		//GPU data for the material
		BufferHandle m_constantBuffer;

		DescriptorLayoutHandle m_descriptorLayout;
		DescriptorSetDesc m_descriptorSetDesc;
		DescriptorSetHandle m_descriptorHandle;
	};
	using MaterialHandle = std::shared_ptr<Material>;
}