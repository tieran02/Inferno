#include "infPCH.h"
#include "ForwardPass.h"
#include "core.h"
#include "graphics/Graphics.h"
#include "graphics/Bitmap.h"

struct ConstantBufferStruct cbVS;

ForwardPass::ForwardPass()
{

}

void ForwardPass::Init(GFX::IDevice* device, IFramebuffer* fb)
{
	//Pre allocate constant buffers for the max object count
	constexpr uint32_t MAX_OBJECT_COUNT = 32;

	m_matrixBuffers.resize(MAX_OBJECT_COUNT);
	for (int i; i < MAX_OBJECT_COUNT; i++)
	{
		GFX::BufferDesc constantBufferDesc;
		constantBufferDesc.usage = GFX::BufferUsage::CONSTANT;
		constantBufferDesc.access = GFX::CpuVisible::WRITE;
		constantBufferDesc.byteSize = sizeof(ConstantBufferStruct);
		constantBufferDesc.name = std::format("MatrixConstantBuffer_{0}", i);
		constantBufferDesc.onlyValidDuringCommandList = true;

		m_matrixBuffers[i].m_constantBuffer = device->CreateBuffer(constantBufferDesc);
	}

	m_view.SetViewport(GFX::Viewport(0, 0, (float)fb->GetInfo().width, (float)fb->GetInfo().height));
	m_view.SetScissor(GFX::Rect(0, 0, fb->GetInfo().width, fb->GetInfo().height));
	m_view.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
	m_view.LookAt(glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0, 1, 0));


	GFX::SamplerDesc samplerDesc;
	m_sampler = device->CreateSampler(samplerDesc);

	//Texture is temp till we pass in materials to MeshSet
	GFX::Bitmap bitmap;
	bitmap.Load("data/textures/uvTest.png");

	GFX::TextureDesc textureDesc;
	textureDesc.width = bitmap.Width();
	textureDesc.height = bitmap.Height();
	textureDesc.format = bitmap.GetFormat();
	textureDesc.name = L"Test texture";
	textureDesc.initialState = (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::PIXEL_SHADER_RESOURCE;
	m_texture = device->CreateTexture(textureDesc);
	device->ImmediateSubmit([=](GFX::ICommandList* cmd)
		{
			cmd->WriteTexture(m_texture.get(), bitmap);
		});

	CreatePipeline(device, fb);
}


void ForwardPass::CreatePipeline(GFX::IDevice* device, IFramebuffer* fb)
{
	//descriptos
	GFX::DescriptorLayoutDesc descriptorDesc;
	descriptorDesc.VS[0].registerSpace = 0;
	descriptorDesc.VS[0].slot = 0;
	descriptorDesc.VS[0].type = GFX::ResourceType::CONSTANTBUFFER;
	descriptorDesc.PS[0].registerSpace = 0;
	descriptorDesc.PS[0].slot = 0;
	descriptorDesc.PS[0].type = GFX::ResourceType::TEXTURE_SRV;
	descriptorDesc.PS[1].registerSpace = 0;
	descriptorDesc.PS[1].slot = 0;
	descriptorDesc.PS[1].type = GFX::ResourceType::SAMPLER;
	m_descriptorHandle = device->CreateDescriptorLayout(descriptorDesc);


	GFX::ShaderDesc vertexShaderDesc;
	vertexShaderDesc.shaderType = GFX::ShaderType::Vertex;
	vertexShaderDesc.shaderPath = "data/shaders/constantBuffer.vert.dxil";
	GFX::ShaderHandle vertexShader = device->CreateShader(vertexShaderDesc);

	GFX::ShaderDesc pixelShaderDesc;
	pixelShaderDesc.shaderType = GFX::ShaderType::Pixel;
	pixelShaderDesc.shaderPath = "data/shaders/constantBuffer.pixel.dxil";
	GFX::ShaderHandle pixelShader = device->CreateShader(pixelShaderDesc);


	//Pipeline
	GFX::GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.VS = vertexShader;
	pipelineDesc.PS = pixelShader;
	pipelineDesc.depthStencilState.depthTestEnable = false;
	pipelineDesc.depthStencilState.depthWriteEnable = false;
	pipelineDesc.rasterState.cullMode = GFX::RasterCullMode::NONE;
	pipelineDesc.descriptorLayout = m_descriptorHandle;

	pipelineDesc.inputLayoutDesc.emplace_back("POSITION", GFX::Format::RGBA32_FLOAT);
	pipelineDesc.inputLayoutDesc.emplace_back("TEXCOORD", GFX::Format::RGBA32_FLOAT);
	pipelineDesc.inputLayoutDesc.emplace_back("COLOR", GFX::Format::RGBA32_FLOAT);

	m_pipeline = device->CreateGraphicsPipeline(pipelineDesc, fb);


	for (int i = 0; i < m_matrixBuffers.size(); i++)
	{
		GFX::DescriptorSetDesc descriptorSetDesc;
		descriptorSetDesc.VS[0] = GFX::DescriptorSetItem::ConstantBuffer(0, m_matrixBuffers[i].m_constantBuffer.get());
		descriptorSetDesc.PS[0] = GFX::DescriptorSetItem::SRV(0, m_texture.get());
		descriptorSetDesc.PS[1] = GFX::DescriptorSetItem::Sampler(0, m_sampler.get());
		m_matrixBuffers[i].m_descriptorHandle = device->CreateDescriptorSet(descriptorSetDesc, m_descriptorHandle.get());
	}
	

}

void ForwardPass::Prepare(ICommandList* commandList, const View& view, MeshInstance** meshInstances, uint32_t meshCount)
{
	m_meshInstances = meshInstances;
	m_meshCount = meshCount;
	m_view = view;

	//View and projection should really be in thier own buffer as its not per mesh
	for (int i = 0; i < meshCount; ++i)
	{
		const MeshInstance* instance = meshInstances[i];
		const MeshInfo* meshInfo = instance->mesh;

		cbVS.projection = m_view.GetProjectionMatrix();
		cbVS.view = m_view.GetViewMatrix();
		cbVS.model = m_meshInstances[i]->transform.GetWorldMatrix();

		commandList->WriteBuffer(m_matrixBuffers[i].m_constantBuffer.get(), &cbVS, sizeof(cbVS));
	}

}

void ForwardPass::SetState(GraphicsState& state)
{
	state.pipeline = m_pipeline.get();
	state.view = &m_view;
}

void ForwardPass::GetMeshInstances(MeshInstance**& instances, uint32_t& meshCount)
{
	instances = m_meshInstances;
	meshCount = m_meshCount;
}

void ForwardPass::OnMeshInstanceRender(uint32_t meshInstanceIndex, GraphicsState& state)
{
	INF_ASSERT(meshInstanceIndex < m_matrixBuffers.size(), "Exceeded MAX_OBJECT_COUNT");
	state.descriptorSet = m_matrixBuffers[meshInstanceIndex].m_descriptorHandle.get();
}