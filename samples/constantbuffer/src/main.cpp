#include "infPCH.h"
#include "core/Log.h"
#include "core/Assert.h"
#include "window/IWindow.h"
#include "graphics/interface/device.h"
#include "graphics/graphicDefines.h"
#include "graphics/interface/Shader.h"
#include "graphics/interface/deviceManager.h"
#include "graphics/interface/Texture.h"
#include "graphics/interface/Buffer.h"


#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "graphics/Bitmap.h"
#include "core/Transform.h"
#include "graphics/View.h"

using namespace INF;

struct vertex
{
	alignas(16) glm::vec2 pos;
	alignas(16) glm::vec2 uv;
	alignas(16) glm::vec3 color;
};

std::array<vertex, 3> verts{
	vertex{{0.0, 0.5},	 {0.5, 0.0},	{ 1.0f, 0.0, 0.0f }},
	vertex{{0.5, -0.5},	 {1.0, 1.0},	{0.0f, 1.0, 0.0f}},
	vertex{{-0.5, -0.5}, {0.0, 1.0},	{0.0f, 0.0, 1.0f}},
};
std::array<uint16_t, 3> indices{
	0,1,2
};

struct ConstantBufferStruct
{
	glm::mat4 model{ 1 };
	glm::mat4 view{ 1 };;
	glm::mat4 projection{ 1 };;
} cbVS;

int main()
{
	GFX::DeviceCreationParameters deviceInfo;
	deviceInfo.enableDebugValidation = true;
	deviceInfo.vsync = false;
	deviceInfo.backBufferWidth = 1280;
	deviceInfo.backBufferHeight = 720;

	Log::Info("Starting editor");
	std::unique_ptr<IWindow> window = IWindow::Create("Hello Window", deviceInfo.backBufferWidth, deviceInfo.backBufferHeight);

	GFX::DeviceManagerHandle deviceManager = GFX::IDeviceManager::Create(GFX::API::D3D12, deviceInfo);
	deviceManager->CreateDeviceAndSwapChain(window.get(), deviceInfo);

	GFX::IDevice* device = deviceManager->GetDevice();
	GFX::CommandListeHandle cmd = device->CreateCommandList(GFX::CommandQueue::GRAPHICS);

	//framebuffer for each backbuffer
	std::vector<GFX::FramebufferHandle> framebuffers(deviceManager->GetBackBufferCount());
	for (int i = 0; i < framebuffers.size(); ++i)
	{
		GFX::FramebufferDesc fbDesc;
		fbDesc.colorAttachments[0] = GFX::FramebufferAttachment(deviceManager->GetBackBufferTexture(i));
		framebuffers[i] = device->CreateFramebuffer(fbDesc);
	}

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
	GFX::DescriptorLayoutHandle descriptorHandle = device->CreateDescriptorLayout(descriptorDesc);


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
	pipelineDesc.descriptorLayout = descriptorHandle;

	pipelineDesc.inputLayoutDesc.emplace_back("POSITION", GFX::Format::RGBA32_FLOAT);
	pipelineDesc.inputLayoutDesc.emplace_back("TEXCOORD", GFX::Format::RGBA32_FLOAT);
	pipelineDesc.inputLayoutDesc.emplace_back("COLOR", GFX::Format::RGBA32_FLOAT);



	GFX::GraphicsPipelineHandle pipeline = device->CreateGraphicsPipeline(pipelineDesc, framebuffers[0].get());



	//staging buffer
	GFX::BufferDesc indexStagingBufferDesc;
	indexStagingBufferDesc.access = GFX::CpuVisible::WRITE;
	indexStagingBufferDesc.byteSize = static_cast<uint32_t>(sizeof(uint16_t) * indices.size());
	indexStagingBufferDesc.usage = GFX::BufferUsage::GENERIC;
	GFX::BufferHandle indexStagingBuffer = device->CreateBuffer(indexStagingBufferDesc);
	void* dest = device->MapBuffer(indexStagingBuffer.get());
	memcpy(dest, indices.data(), indexStagingBufferDesc.byteSize);

	//create index buffer
	GFX::IndexBufferDesc indexBufferDesc;
	indexBufferDesc.access = GFX::CpuVisible::NONE;
	indexBufferDesc.format = GFX::Format::R16_UINT;
	indexBufferDesc.byteSize = static_cast<uint32_t>(sizeof(uint16_t) * indices.size());
	GFX::IndexBufferHandle indexBuffer = device->CreateIndexBuffer(indexBufferDesc);

	//staging buffer
	GFX::BufferDesc vertexStagingBufferDesc;
	vertexStagingBufferDesc.access = GFX::CpuVisible::WRITE;
	vertexStagingBufferDesc.byteSize = static_cast<uint32_t>(sizeof(vertex) * verts.size());
	vertexStagingBufferDesc.usage = GFX::BufferUsage::GENERIC;
	GFX::BufferHandle vertexStagingBuffer = device->CreateBuffer(vertexStagingBufferDesc);
	dest = device->MapBuffer(vertexStagingBuffer.get());
	memcpy(dest, verts.data(), vertexStagingBufferDesc.byteSize);

	//create vertex buffer
	GFX::VertexBufferDesc vertexBufferDesc;
	vertexBufferDesc.access = GFX::CpuVisible::NONE;
	vertexBufferDesc.byteSize = static_cast<uint32_t>(sizeof(vertex) * verts.size());
	vertexBufferDesc.strideInBytes = sizeof(vertex);
	GFX::VertexBufferHandle vertexBuffer = device->CreateVertexBuffer(vertexBufferDesc);

	//Texture
	GFX::Bitmap bitmap;
	bitmap.Load("data/textures/uvTest.png");

	GFX::TextureDesc textureDesc;
	textureDesc.width = bitmap.Width();
	textureDesc.height = bitmap.Height();
	textureDesc.format = bitmap.GetFormat();
	textureDesc.name = L"Test texture";
	textureDesc.initialState = (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::PIXEL_SHADER_RESOURCE;
	GFX::TextureHandle texture = device->CreateTexture(textureDesc);


	device->ImmediateSubmit([=](GFX::ICommandList* cmd)
		{
			cmd->CopyBuffer(vertexBuffer->GetBuffer(), 0, vertexStagingBuffer.get(), 0, vertexStagingBufferDesc.byteSize);
			cmd->CopyBuffer(indexBuffer->GetBuffer(), 0, indexStagingBuffer.get(), 0, indexStagingBufferDesc.byteSize);
			cmd->WriteTexture(texture.get(), bitmap);
		});
	//reset staging buffer to free resources
	indexStagingBuffer.reset();
	vertexStagingBuffer.reset();


	//Create a descriptor set that contains the actual buffer to be bound
	GFX::BufferDesc constantBufferDesc;
	constantBufferDesc.usage = GFX::BufferUsage::CONSTANT;
	constantBufferDesc.access = GFX::CpuVisible::WRITE;
	constantBufferDesc.byteSize = sizeof(ConstantBufferStruct);
	constantBufferDesc.name = "Constant Buffer";
	GFX::BufferHandle constantBuffer = device->CreateBuffer(constantBufferDesc);

	ConstantBufferStruct* matrixData = (ConstantBufferStruct*)device->MapBuffer(constantBuffer.get());
	memcpy(dest, &cbVS, constantBufferDesc.byteSize);
	GFX::View view(70.0f, (float)deviceInfo.backBufferWidth / (float)deviceInfo.backBufferHeight, 0.1f, 100.0);
	view.SetViewport(GFX::Viewport(0, 0, (float)deviceInfo.backBufferWidth, (float)deviceInfo.backBufferHeight));
	view.SetScissor(GFX::Rect(0, 0, deviceInfo.backBufferWidth, deviceInfo.backBufferHeight));
	view.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
	view.LookAt(glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0, 1, 0));

	matrixData->projection = view.GetProjectionMatrix();
	matrixData->view = view.GetViewMatrix();


	GFX::SamplerDesc samplerDesc;
	GFX::SamplerHandle sampler = device->CreateSampler(samplerDesc);

	GFX::DescriptorSetDesc descriptorSetDesc;
	descriptorSetDesc.VS[0] = GFX::DescriptorSetItem::ConstantBuffer(0, constantBuffer.get());
	descriptorSetDesc.PS[0] = GFX::DescriptorSetItem::SRV(0, texture.get());
	descriptorSetDesc.PS[1] = GFX::DescriptorSetItem::Sampler(0, sampler.get());
	GFX::DescriptorSetHandle descriptorSet = device->CreateDescriptorSet(descriptorSetDesc, descriptorHandle.get());

	Input input;
	window->SetInputKeyRegisterCallback(input.GetRegisterKeyFn());
	window->SetInputMouseButtonRegisterCallback(input.GetRegisterMouseButtonFn());
	window->SetInputMouseCursorRegisterCallback(input.GetRegisterMouseCursorFn());

	int shouldClose = false;
	window->SetCloseCallBack([&shouldClose]
		{
			shouldClose = true;
		});

	INF::Transform transform;
	while (!shouldClose)
	{
		typedef std::chrono::high_resolution_clock clock;
		typedef std::chrono::duration<float, std::milli> duration;
		static clock::time_point start = clock::now();
		float elapsed = (duration(clock::now() - start)).count();

		transform.SetPosition(glm::vec3(sinf(elapsed * 0.001f), 0.0f, 0.0f));
		transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 0.01f);
		transform.UpdateTransform();
		matrixData->model = transform.GetWorldMatrix();

		//move camera with WASD
		if (input.IsKeyDown(KeyCode::A))
		{
			view.Translate(-VectorRight * 0.1f);
		}
		if (input.IsKeyDown(KeyCode::D))
		{
			view.Translate(VectorRight * 0.1f);
		}
		if (input.IsKeyDown(KeyCode::W))
		{
			view.Translate(VectorUp * 0.1f);
		}
		if (input.IsKeyDown(KeyCode::S))
		{
			view.Translate(-VectorUp * 0.1f);
		}
		matrixData->view = view.GetViewMatrix();


		GFX::GraphicsState graphicsState;
		graphicsState.pipeline = pipeline.get();
		graphicsState.framebuffer = framebuffers[deviceManager->GetCurrentBackBufferIndex()].get();
		graphicsState.descriptorSet = descriptorSet.get();
		graphicsState.vertexBuffer = vertexBuffer.get();
		graphicsState.indexBuffer = indexBuffer.get();
		graphicsState.view = &view;

		window->PollEvents();
		input.Update();

		deviceManager->BeginFrame();

		cmd->Open();

		cmd->SetGraphicsState(graphicsState);

		cmd->ClearColor(deviceManager->GetCurrentBackBufferTexture(), GFX::Color(0.2f, 0.2f, 0.2f, 1.0f));

		cmd->Draw(3, 1, 0, 0);

		cmd->Close();

		device->ExecuteCommandLists(cmd.get(), 1);

		deviceManager->Present();

		device->WaitForIdle();

		if (input.IsKeyRelease(KeyCode::Escape))
			shouldClose = true;
	}


	Log::Info("Closing editor");

}