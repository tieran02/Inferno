#pragma once
#include "graphics/RenderPass.h"
#include "graphics/View.h"

using namespace INF;
using namespace INF::GFX;

struct ConstantBufferStruct
{
	glm::mat4 model{ 1 };
	glm::mat4 view{ 1 };;
	glm::mat4 projection{ 1 };;
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
	void OnMeshInstanceRender(const MeshInstance* instance) override;

private:
	void CreatePipeline(GFX::IDevice* device, IFramebuffer* fb);

	GraphicsPipelineHandle m_pipeline;
	DescriptorLayoutHandle m_descriptorHandle;
	DescriptorSetHandle m_descriptorSet;

	ConstantBufferStruct* m_matrixData;
	BufferHandle m_constantBuffer;
	SamplerHandle m_sampler;
	TextureHandle m_texture;
	MeshInstance** m_meshInstances;
	uint32_t m_meshCount;
	View m_view;
};