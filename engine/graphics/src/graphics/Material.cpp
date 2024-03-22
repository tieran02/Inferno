#include "infPCH.h"
#include "graphics/Material.h"
#include "graphics/interface/device.h"


INF::GFX::Material::Material(std::string_view name) : m_name(name)
{

}

void INF::GFX::Material::RegisterTexture(std::string_view name, uint8_t textureSlot)
{
	m_textureSlotMap.emplace(name, textureSlot);
}

void INF::GFX::Material::SetData(std::string_view name, const float& data)
{
	SetDataT(name, data);
}

void INF::GFX::Material::SetData(std::string_view name, const glm::mat4& data)
{
	SetDataT(name, data);
}

void INF::GFX::Material::SetData(std::string_view name, const glm::mat3& data)
{
	SetDataT(name, data);
}

void INF::GFX::Material::SetData(std::string_view name, const glm::vec4& data)
{
	SetDataT(name, data);
}

void INF::GFX::Material::SetData(std::string_view name, const glm::vec3& data)
{
	SetDataT(name, data);
}

void INF::GFX::Material::SetData(std::string_view name, const glm::vec2& data)
{
	SetDataT(name, data);
}

void INF::GFX::Material::SetData(std::string_view name, const uint32_t& data)
{
	SetDataT(name, data);
}

void INF::GFX::Material::SetData(std::string_view name, const TextureHandle& texture, const SamplerHandle& sampler)
{
	std::string str = std::string(name);
	auto it = m_textureSlotMap.find(str);
	INF_ASSERT(it != m_textureSlotMap.end(), std::format("No texture exists with name:{0}", name));

	int textureSlot = it->second + 1;
	int samplerSlot = textureSlot + MAX_TEXTURE_COUNT_PER_MATERIAL;
	m_textures[textureSlot] = texture;
	m_samplers[textureSlot] = sampler;

	m_descriptorSetDesc.PS[textureSlot] = GFX::DescriptorSetItem::SRV(textureSlot, texture.get());
	m_descriptorSetDesc.PS[samplerSlot] = GFX::DescriptorSetItem::Sampler(textureSlot, sampler.get());
}

void INF::GFX::Material::CreateBuffer(IDevice* device, size_t size)
{
	GFX::BufferDesc constantBufferDesc;
	constantBufferDesc.usage = GFX::BufferUsage::CONSTANT;
	constantBufferDesc.access = GFX::CpuVisible::WRITE;
	constantBufferDesc.byteSize = size;
	constantBufferDesc.name = std::format(L"MaterialConstantBuffer_{0}", std::wstring(m_name.begin(), m_name.end()));
	constantBufferDesc.onlyValidDuringCommandList = false;

	m_constantBuffer = device->CreateBuffer(constantBufferDesc);
	m_constantBufferSize = size;
	m_constantBufferDataPtr = (uint8_t*)device->MapBuffer(m_constantBuffer.get());

	m_descriptorSetDesc.PS[0] = GFX::DescriptorSetItem::ConstantBuffer(0, m_constantBuffer.get());

	m_descriptorLayout = device->CreateDescriptorLayout(DescriptorLayout());
}

INF::GFX::DescriptorLayoutDesc INF::GFX::Material::DescriptorLayout()
{
	GFX::DescriptorLayoutDesc matDescriptorDesc;
	matDescriptorDesc.PS[0].registerSpace = 0;
	matDescriptorDesc.PS[0].slot = 2;
	matDescriptorDesc.PS[0].type = GFX::ResourceType::CONSTANTBUFFER; //Material constant Buffer

	for (int i = 0; i < MAX_TEXTURE_COUNT_PER_MATERIAL; ++i)
	{
		matDescriptorDesc.PS[i + 1].registerSpace = 0;
		matDescriptorDesc.PS[i + 1].slot = i;
		matDescriptorDesc.PS[i + 1].type = GFX::ResourceType::TEXTURE_SRV; //Texture

		matDescriptorDesc.PS[i + 1 + MAX_TEXTURE_COUNT_PER_MATERIAL].registerSpace = 0;
		matDescriptorDesc.PS[i + 1 + MAX_TEXTURE_COUNT_PER_MATERIAL].slot = i;
		matDescriptorDesc.PS[i + 1 + MAX_TEXTURE_COUNT_PER_MATERIAL].type = GFX::ResourceType::SAMPLER;  //Texture sampler
	}

	return matDescriptorDesc;
}

void INF::GFX::Material::UpdateDescriptorSet(IDevice* device)
{
	m_descriptorHandle = device->CreateDescriptorSet(m_descriptorSetDesc, m_descriptorLayout.get());
}
