#include "infPCH.h"
#include "core/Log.h"
#include "core/Assert.h"
#include "window/IWindow.h"
#include "graphics/interface/device.h"
#include "graphics/graphicDefines.h"
#include "graphics/interface/Shader.h"
#include "graphics/interface/deviceManager.h"
#include "graphics/View.h"

using namespace INF;

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

	GFX::ShaderDesc vertexShaderDesc;
	vertexShaderDesc.shaderType = GFX::ShaderType::Vertex;
	vertexShaderDesc.shaderPath = "data/shaders/triangle.vert.dxil";
	GFX::ShaderHandle vertexShader = device->CreateShader(vertexShaderDesc);

	GFX::ShaderDesc pixelShaderDesc;
	pixelShaderDesc.shaderType = GFX::ShaderType::Pixel;
	pixelShaderDesc.shaderPath = "data/shaders/triangle.pixel.dxil";
	GFX::ShaderHandle pixelShader = device->CreateShader(pixelShaderDesc);

	//Empty root sig/descriptor layout
	GFX::DescriptorLayoutDesc descriptorLayoutDesc;
	GFX::DescriptorLayoutHandle descriptorLayoutHandle = device->CreateDescriptorLayout(descriptorLayoutDesc);

	//Pipeline
	GFX::GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.VS = vertexShader;
	pipelineDesc.PS = pixelShader;
	pipelineDesc.depthStencilState.depthTestEnable = false;
	pipelineDesc.depthStencilState.depthWriteEnable = false;
	pipelineDesc.rasterState.cullMode = GFX::RasterCullMode::NONE;
	pipelineDesc.descriptorLayout = descriptorLayoutHandle;
	GFX::GraphicsPipelineHandle pipeline = device->CreateGraphicsPipeline(pipelineDesc, framebuffers[0].get());

	Input input;
	window->SetInputKeyRegisterCallback(input.GetRegisterKeyFn());
	window->SetInputMouseButtonRegisterCallback(input.GetRegisterMouseButtonFn());
	window->SetInputMouseCursorRegisterCallback(input.GetRegisterMouseCursorFn());

	GFX::View view;
	view.SetViewport(GFX::Viewport(0, 0, deviceInfo.backBufferWidth, deviceInfo.backBufferHeight));
	view.SetScissor(GFX::Rect(0, 0, deviceInfo.backBufferWidth, deviceInfo.backBufferHeight));

	int shouldClose = false;
	window->SetCloseCallBack([&shouldClose]
	{
		shouldClose = true;
	});

	while (!shouldClose)
	{
		GFX::GraphicsState graphicsState;
		graphicsState.pipeline = pipeline.get();
		graphicsState.framebuffer = framebuffers[deviceManager->GetCurrentBackBufferIndex()].get();
		graphicsState.view = &view;

		window->PollEvents();
		input.Update();

		cmd->Open();
		
		cmd->SetGraphicsState(graphicsState);
		
		cmd->ClearColor(deviceManager->GetCurrentBackBufferTexture(), GFX::Color(0.2f, 0.2f, 0.2f, 1.0f));
		
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

	pipeline.reset();
	descriptorLayoutHandle.reset();
	pixelShader.reset();
	vertexShader.reset();
	framebuffers.clear();
	cmd.reset();
	deviceManager.reset();
	Log::Info("Closing editor");

}