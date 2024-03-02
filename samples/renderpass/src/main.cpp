#include "infPCH.h"
#include "Core.h"
#include "graphics/Graphics.h"
#include "ForwardPass.h"

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

using namespace INF;

void CreateVertexBuffer(GFX::IDevice* device, GFX::VertexBufferHandle& vertexBuffer, GFX::IndexBufferHandle& indexBuffer)
{
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
	indexBuffer = device->CreateIndexBuffer(indexBufferDesc);

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
	vertexBuffer = device->CreateVertexBuffer(vertexBufferDesc);

	device->ImmediateSubmit([=](GFX::ICommandList* cmd)
		{
			cmd->CopyBuffer(vertexBuffer->GetBuffer(), 0, vertexStagingBuffer.get(), 0, vertexStagingBufferDesc.byteSize);
			cmd->CopyBuffer(indexBuffer->GetBuffer(), 0, indexStagingBuffer.get(), 0, indexStagingBufferDesc.byteSize);
		});
}

int main()
{
	GFX::DeviceCreationParameters deviceInfo;
	deviceInfo.enableDebugValidation = true;
	deviceInfo.vsync = false;
	deviceInfo.backBufferWidth = 1280;
	deviceInfo.backBufferHeight = 720;

	Log::Info("Starting editor");
	std::unique_ptr<IWindow> window = IWindow::Create("Render Pass", deviceInfo.backBufferWidth, deviceInfo.backBufferHeight);

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

	ForwardPass forwardPass;
	forwardPass.Init(device, framebuffers[0].get());

	Input input;
	window->SetInputKeyRegisterCallback(input.GetRegisterKeyFn());
	window->SetInputMouseButtonRegisterCallback(input.GetRegisterMouseButtonFn());
	window->SetInputMouseCursorRegisterCallback(input.GetRegisterMouseCursorFn());

	int shouldClose = false;
	window->SetCloseCallBack([&shouldClose]
		{
			shouldClose = true;
		});

	GFX::View view;
	view.SetViewport(GFX::Viewport(0, 0, deviceInfo.backBufferWidth, deviceInfo.backBufferHeight));
	view.SetScissor(GFX::Rect(0, 0, deviceInfo.backBufferWidth, deviceInfo.backBufferHeight));


	GFX::VertexBufferHandle vertexBuffer;
	GFX::IndexBufferHandle indexBuffer;
	CreateVertexBuffer(device, vertexBuffer, indexBuffer);

	MeshInfo meshInfo;
	meshInfo.buffer.vertexBuffer = vertexBuffer;
	meshInfo.buffer.indexBuffer = indexBuffer;
	meshInfo.indexOffset = 0;
	meshInfo.numVertices = verts.size();
	meshInfo.numIndices = indices.size();

	MeshInstance meshInstance;
	meshInstance.mesh = &meshInfo;
	meshInstance.instanceOffset = 0;

	MeshInstance meshInstance1;
	meshInstance1.mesh = &meshInfo;
	meshInstance1.instanceOffset = 0;

	
	std::array<MeshInstance*, 2> meshes{&meshInstance, &meshInstance1};
	while (!shouldClose)
	{
		window->PollEvents();
		input.Update();

		typedef std::chrono::high_resolution_clock clock;
		typedef std::chrono::duration<float, std::milli> duration;
		static clock::time_point start = clock::now();
		float elapsed = (duration(clock::now() - start)).count();

		glm::vec3 posOffset = glm::vec3(sinf(elapsed * 0.001f), 0.0f, 0.0f);
		meshInstance.transform.SetPosition(posOffset);
		meshInstance.transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 0.01f);
		meshInstance.transform.UpdateTransform();

		meshInstance1.transform.SetPosition(-posOffset);
		meshInstance1.transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 0.01f);
		meshInstance1.transform.UpdateTransform();

		deviceManager->BeginFrame();

		cmd->Open();

		
		//need better barrier managment
		cmd->Transition(deviceManager->GetCurrentBackBufferTexture(), (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::PRESENT, (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::RENDER_TARGET);
		cmd->ClearColor(deviceManager->GetCurrentBackBufferTexture(), GFX::Color(0.2f, 0.2f, 0.2f, 1.0f));
		cmd->Transition(deviceManager->GetCurrentBackBufferTexture(), (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::RENDER_TARGET, (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::PRESENT);


		forwardPass.Prepare(cmd.get(), view, meshes.data(), meshes.size());

		auto frame = deviceManager->GetCurrentBackBufferIndex();
		IFramebuffer* backBuffer = framebuffers[frame].get();
		forwardPass.Render(cmd.get(), backBuffer);

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