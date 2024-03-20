#pragma once
#include "graphics/RenderPass.h"
#include "graphics/View.h"

using namespace INF;
using namespace INF::GFX;

struct SceneBufferStruct
{
	glm::mat4 view{ 1 };
	glm::mat4 projection{ 1 };
	glm::vec4 ambientColor{ 0.2f, 0.2f, 0.2f, 1.0f };
	glm::vec4 lightColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec4 lightDir{ 0.25f, 0.5f, 0.6f, 1.0f };
};

struct ConstantBufferStruct
{
	glm::mat4 model{ 1 };
};

struct PersistantMappedBuffer
{
	BufferHandle m_constantBuffer;

	DescriptorSetHandle m_descriptorHandle;
};


class ForwardPass : public IGeometryPass
{
public:
	ForwardPass();
	void Init(GFX::IDevice* device, IFramebuffer* fb);

	std::string_view GetName() override { return "ForwardPass"; }
	void Prepare(ICommandList* commandList, const View& view, MeshInstance** meshInstances, uint32_t meshCount) override;
	void SetState(GraphicsState& state) override;

	void GetMeshInstances(MeshInstance**& instances, uint32_t& meshCount) override;
	void OnMeshInstanceRender(uint32_t meshInstanceIndex, GraphicsState& state) override;

private:
	void CreatePipeline(GFX::IDevice* device, IFramebuffer* fb);

	GraphicsPipelineHandle m_pipeline;
	DescriptorLayoutHandle m_viewDescriptorLayoutHandle;
	DescriptorLayoutHandle m_meshDescriptorHandle;

	BufferHandle m_viewConstantBuffer;
	DescriptorSetHandle m_viewDescriptorHandle;
	std::vector<PersistantMappedBuffer> m_matrixBuffers;

	SamplerHandle m_sampler;
	TextureHandle m_texture;
	MeshInstance** m_meshInstances;
	uint32_t m_meshCount;
	View m_view;
};