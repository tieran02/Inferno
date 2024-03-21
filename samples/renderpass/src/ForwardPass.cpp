#include "infPCH.h"
#include "ForwardPass.h"
#include "core.h"
#include "graphics/Graphics.h"
#include "graphics/Bitmap.h"
#include "imgui.h"

ConstantBufferStruct cbVS;
SceneBufferStruct cbScene;

ForwardPass::ForwardPass()
{

}

void ForwardPass::Init(GFX::IDevice* device, IFramebuffer* fb)
{
	//Pre allocate constant buffers for the max object count
	constexpr uint32_t MAX_OBJECT_COUNT = 32;

	m_matrixBuffers.resize(MAX_OBJECT_COUNT);
	for (int i = 0; i < MAX_OBJECT_COUNT; i++)
	{
		GFX::BufferDesc constantBufferDesc;
		constantBufferDesc.usage = GFX::BufferUsage::CONSTANT;
		constantBufferDesc.access = GFX::CpuVisible::WRITE;
		constantBufferDesc.byteSize = sizeof(ConstantBufferStruct);
		constantBufferDesc.name = std::format("MatrixConstantBuffer_{0}", i);
		constantBufferDesc.onlyValidDuringCommandList = true;

		m_matrixBuffers[i].m_constantBuffer = device->CreateBuffer(constantBufferDesc);
	}

	GFX::BufferDesc viewConstantBufferDesc;
	viewConstantBufferDesc.usage = GFX::BufferUsage::CONSTANT;
	viewConstantBufferDesc.access = GFX::CpuVisible::WRITE;
	viewConstantBufferDesc.byteSize = sizeof(ConstantBufferStruct);
	viewConstantBufferDesc.name = "ViewConstantBuffer";
	viewConstantBufferDesc.onlyValidDuringCommandList = true;
	m_viewConstantBuffer = device->CreateBuffer(viewConstantBufferDesc);


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
	GFX::DescriptorLayoutDesc viewDescriptorDesc;
	viewDescriptorDesc.VS[0].registerSpace = 0;
	viewDescriptorDesc.VS[0].slot = 0;
	viewDescriptorDesc.VS[0].type = GFX::ResourceType::CONSTANTBUFFER; //ViewProjecton buffer
	viewDescriptorDesc.PS[0].registerSpace = 0;
	viewDescriptorDesc.PS[0].slot = 0;
	viewDescriptorDesc.PS[0].type = GFX::ResourceType::CONSTANTBUFFER; //ViewProjecton buffer
	m_viewDescriptorLayoutHandle = device->CreateDescriptorLayout(viewDescriptorDesc);

	GFX::DescriptorLayoutDesc meshDescriptorDesc;
	meshDescriptorDesc.VS[0].registerSpace = 0;
	meshDescriptorDesc.VS[0].slot = 1;
	meshDescriptorDesc.VS[0].type = GFX::ResourceType::CONSTANTBUFFER; //Model Buffer
	meshDescriptorDesc.PS[0].registerSpace = 0;
	meshDescriptorDesc.PS[0].slot = 0;
	meshDescriptorDesc.PS[0].type = GFX::ResourceType::TEXTURE_SRV; //Texture
	meshDescriptorDesc.PS[1].registerSpace = 0;
	meshDescriptorDesc.PS[1].slot = 0;
	meshDescriptorDesc.PS[1].type = GFX::ResourceType::SAMPLER;  //Texture sampler
	m_meshDescriptorHandle = device->CreateDescriptorLayout(meshDescriptorDesc);


	GFX::ShaderDesc vertexShaderDesc;
	vertexShaderDesc.shaderType = GFX::ShaderType::Vertex;
	vertexShaderDesc.shaderPath = "data/shaders/forwardPass.vert.dxil";
	vertexShaderDesc.entryName = "VSmain";
	GFX::ShaderHandle vertexShader = device->CreateShader(vertexShaderDesc);

	GFX::ShaderDesc pixelShaderDesc;
	pixelShaderDesc.shaderType = GFX::ShaderType::Pixel;
	pixelShaderDesc.shaderPath = "data/shaders/forwardPass.pixel.dxil";
	vertexShaderDesc.entryName = "PSmain";
	GFX::ShaderHandle pixelShader = device->CreateShader(pixelShaderDesc);


	//Pipeline
	GFX::GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.VS = vertexShader;
	pipelineDesc.PS = pixelShader;
	pipelineDesc.depthStencilState.depthTestEnable = true;
	pipelineDesc.depthStencilState.depthWriteEnable = true;
	pipelineDesc.depthStencilState.depthFunc = ComparisonFunc::LESS_OR_EQUAL;
	pipelineDesc.rasterState.cullMode = GFX::RasterCullMode::NONE;
	pipelineDesc.descriptorLayoutSet = { m_viewDescriptorLayoutHandle, m_meshDescriptorHandle};

	pipelineDesc.inputLayoutDesc.emplace_back("POSITION", GFX::Format::RGB32_FLOAT);
	pipelineDesc.inputLayoutDesc.emplace_back("NORMAL", GFX::Format::RGB32_FLOAT);
	pipelineDesc.inputLayoutDesc.emplace_back("TEXCOORD", GFX::Format::RG32_FLOAT);

	m_pipeline = device->CreateGraphicsPipeline(pipelineDesc, fb);


	for (int i = 0; i < m_matrixBuffers.size(); i++)
	{
		GFX::DescriptorSetDesc descriptorSetDesc;
		descriptorSetDesc.VS[0] = GFX::DescriptorSetItem::ConstantBuffer(0, m_matrixBuffers[i].m_constantBuffer.get());
		descriptorSetDesc.PS[0] = GFX::DescriptorSetItem::SRV(0, m_texture.get());
		descriptorSetDesc.PS[1] = GFX::DescriptorSetItem::Sampler(0, m_sampler.get());
		m_matrixBuffers[i].m_descriptorHandle = device->CreateDescriptorSet(descriptorSetDesc, m_meshDescriptorHandle.get());
	}
	
	GFX::DescriptorSetDesc viewDescriptorSetDesc;
	viewDescriptorSetDesc.VS[0] = GFX::DescriptorSetItem::ConstantBuffer(0, m_viewConstantBuffer.get());
	viewDescriptorSetDesc.PS[0] = GFX::DescriptorSetItem::ConstantBuffer(0, m_viewConstantBuffer.get());
	m_viewDescriptorHandle = device->CreateDescriptorSet(viewDescriptorSetDesc, m_viewDescriptorLayoutHandle.get());

}

void ForwardPass::Prepare(ICommandList* commandList, const View& view, MeshInstance** meshInstances, uint32_t meshCount)
{
	m_meshInstances = meshInstances;
	m_meshCount = meshCount;
	m_view = view;

	//cbScene.lightDir = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	//cbScene.ambientColor = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);

	ImGui::Begin("Light");
	ImGui::DragFloat4("Direction", &cbScene.lightDir.r, 0.05, -1.0f, 1.0f);
	ImGui::ColorEdit4("Light Color", &cbScene.lightColor.r);
	ImGui::ColorEdit4("Ambient Color", &cbScene.ambientColor.r);
	ImGui::End();

	cbScene.projection = m_view.GetProjectionMatrix();
	cbScene.view = m_view.GetViewMatrix();
	commandList->WriteBuffer(m_viewConstantBuffer.get(), &cbScene, sizeof(cbScene));

	//View and projection should really be in thier own buffer as its not per mesh
	for (int i = 0; i < meshCount; ++i)
	{
		const MeshInstance* instance = meshInstances[i];
		const MeshInfo* meshInfo = instance->mesh;

		cbVS.model = m_meshInstances[i]->transform.GetWorldMatrix();

		commandList->WriteBuffer(m_matrixBuffers[i].m_constantBuffer.get(), &cbVS, sizeof(cbVS));
	}

}

void ForwardPass::SetState(GraphicsState& state)
{
	state.pipeline = m_pipeline.get();
	state.view = &m_view;
	state.descriptorSetGroup[0] = m_viewDescriptorHandle;
}

void ForwardPass::GetMeshInstances(MeshInstance**& instances, uint32_t& meshCount)
{
	instances = m_meshInstances;
	meshCount = m_meshCount;
}

void ForwardPass::OnMeshInstanceRender(uint32_t meshInstanceIndex, GraphicsState& state)
{
	INF_ASSERT(meshInstanceIndex < m_matrixBuffers.size(), "Exceeded MAX_OBJECT_COUNT");
	state.descriptorSetGroup[1] = m_matrixBuffers[meshInstanceIndex].m_descriptorHandle;
}