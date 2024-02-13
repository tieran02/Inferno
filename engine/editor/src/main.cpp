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

using namespace INF;

struct vertex
{
	float x, y,z,w;
	float r, g, b, a;
};

std::array<vertex, 3> verts{
	vertex{0.0, 0.5, 0,1,			1.0f, 0.0, 0.0f, 1.0f},
	vertex{0.5, -0.5, 0,1,			0.0f, 1.0, 0.0f, 1.0f},
	vertex{-0.5, -0.5, 0,1,			0.0f, 0.0, 1.0f, 1.0f},
};
std::array<uint16_t, 3> indices{
	0,1,2
};

int main()
{
	Log::Info("Starting editor");
	std::unique_ptr<IWindow> window = IWindow::Create("Hello Window", 1280, 720);

	GFX::DeviceCreationParameters deviceInfo;
	deviceInfo.enableDebugValidation = true;

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

	GFX::ShaderDesc vertexShaderDesc;
	vertexShaderDesc.shaderType = GFX::ShaderType::Vertex;
	vertexShaderDesc.shaderPath = "data/shaders/vertexBuffer.vert.dxil";
	GFX::ShaderHandle vertexShader = device->CreateShader(vertexShaderDesc);

	GFX::ShaderDesc pixelShaderDesc;
	pixelShaderDesc.shaderType = GFX::ShaderType::Pixel;
	pixelShaderDesc.shaderPath = "data/shaders/vertexBuffer.pixel.dxil";
	GFX::ShaderHandle pixelShader = device->CreateShader(pixelShaderDesc);


	//Pipeline
	GFX::GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.VS = vertexShader;
	pipelineDesc.PS = pixelShader;
	pipelineDesc.depthStencilState.depthTestEnable = false;
	pipelineDesc.depthStencilState.depthWriteEnable = false;
	pipelineDesc.rasterState.cullMode = GFX::RasterCullMode::NONE;

	pipelineDesc.inputLayoutDesc.emplace_back("POSITION", GFX::Format::RGBA32_FLOAT);
	pipelineDesc.inputLayoutDesc.emplace_back("COLOR", GFX::Format::RGBA32_FLOAT);



	GFX::GraphicsPipelineHandle pipeline = device->CreateGraphicsPipeline(pipelineDesc, framebuffers[0].get());

	GFX::Viewport viewport(0, 0, deviceInfo.backBufferWidth, deviceInfo.backBufferHeight);
	GFX::Rect scissor(0, 0, deviceInfo.backBufferWidth, deviceInfo.backBufferHeight);

	//create vertex buffer
	GFX::VertexBufferDesc vertexBufferDesc;
	vertexBufferDesc.access = GFX::CpuVisible::WRITE;
	vertexBufferDesc.byteSize = sizeof(vertex) * verts.size();
	vertexBufferDesc.strideInBytes = sizeof(vertex);
	GFX::VertexBufferHandle vertexBuffer = device->CreateVertexBuffer(vertexBufferDesc);
	void* dest = device->MapBuffer(vertexBuffer->GetBuffer());
	memcpy(dest, verts.data(), vertexBufferDesc.byteSize);

	//create index buffer
	GFX::IndexBufferDesc indexBufferDesc;
	indexBufferDesc.access = GFX::CpuVisible::WRITE;
	indexBufferDesc.format = GFX::Format::R16_UINT;
	indexBufferDesc.byteSize = sizeof(uint16_t) * indices.size();
	GFX::IndexBufferHandle indexBuffer = device->CreateIndexBuffer(indexBufferDesc);
	dest = device->MapBuffer(indexBuffer->GetBuffer());
	memcpy(dest, indices.data(), indexBufferDesc.byteSize);


	Input input;
	window->SetInputKeyRegisterCallback(input.GetRegisterKeyFn());
	window->SetInputMouseButtonRegisterCallback(input.GetRegisterMouseButtonFn());
	window->SetInputMouseCursorRegisterCallback(input.GetRegisterMouseCursorFn());

	int shouldClose = false;
	window->SetCloseCallBack([&shouldClose]
	{
		shouldClose = true;
	});

	while (!shouldClose)
	{
		typedef std::chrono::high_resolution_clock clock;
		typedef std::chrono::duration<float, std::milli> duration;
		static clock::time_point start = clock::now();
		float elapsed = (duration(clock::now() - start)).count();

		GFX::GraphicsState graphicsState;
		graphicsState.pipeline = pipeline.get();
		graphicsState.framebuffer = framebuffers[deviceManager->GetCurrentBackBufferIndex()].get();
		graphicsState.vertexBuffer = vertexBuffer.get();
		graphicsState.indexBuffer = indexBuffer.get();

		window->PollEvents();
		input.Update();

		cmd->Open();
		cmd->SetViewport(viewport);
		cmd->SetScissor(scissor);
		cmd->SetGraphicsState(graphicsState);


		//cmd->ClearColor(deviceManager->GetCurrentBackBufferTexture(), GFX::Color(0.2f, 0.2f, 0.2f, 1.0f));
		cmd->ClearColor(deviceManager->GetCurrentBackBufferTexture(), GFX::Color((sinf(elapsed * 0.01f) + 1) * 0.5f, 0.5f, 0.2f, 1.0f));

		cmd->Draw(3, 1, 0, 0);


		//transition to present for swapchain
		cmd->Transition(deviceManager->GetCurrentBackBufferTexture(), (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::RENDER_TARGET, (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::PRESENT);

		cmd->Close();

		device->ExecuteCommandLists(cmd.get(), 1);

		deviceManager->Present();

		device->WaitForIdle();

		if (input.IsKeyRelease(KeyCode::Escape))
			shouldClose = true;
	}
	Log::Info("Closing editor");

}