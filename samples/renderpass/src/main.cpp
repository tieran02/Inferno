#include "infPCH.h"

#include "Core.h"
#include "graphics/Graphics.h"
#include "ForwardPass.h"
#include "graphics/MeshGenerator.h"
#include "imgui.h"
#include "graphics/Bitmap.h"
#include "graphics/MaterialLibrary.h"
#include "graphics/GltfLoader.h"


using namespace INF;



int main()
{
	GFX::DeviceCreationParameters deviceInfo;
	deviceInfo.enableDebugValidation = true;
	deviceInfo.vsync = false;
	deviceInfo.backBufferWidth = 1920;
	deviceInfo.backBufferHeight = 1080;

	Log::Info("Starting editor");
	std::unique_ptr<IWindow> window = IWindow::Create("Render Pass", deviceInfo.backBufferWidth, deviceInfo.backBufferHeight);
	Input input;
	window->SetInputKeyRegisterCallback(input.GetRegisterKeyFn());
	window->SetInputMouseButtonRegisterCallback(input.GetRegisterMouseButtonFn());
	window->SetInputMouseCursorRegisterCallback(input.GetRegisterMouseCursorFn());

	int shouldClose = false;
	window->SetCloseCallBack([&shouldClose]
		{
			shouldClose = true;
		});


	GFX::DeviceManagerHandle deviceManager = GFX::IDeviceManager::Create(GFX::API::D3D12, deviceInfo);
	deviceManager->CreateDeviceAndSwapChain(window.get(), deviceInfo);

	GFX::IDevice* device = deviceManager->GetDevice();
	GFX::CommandListeHandle cmd = device->CreateCommandList(GFX::CommandQueue::GRAPHICS);

	GFX::TextureDesc depthDesc;
	depthDesc.width = deviceInfo.backBufferWidth;
	depthDesc.height = deviceInfo.backBufferHeight;
	depthDesc.format = GFX::Format::D32;
	depthDesc.name = L"Depth";
	depthDesc.initialState = (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::DEPTH_WRITE;
	GFX::TextureHandle depthTexture = device->CreateTexture(depthDesc);

	//framebuffer for each backbuffer
	std::vector<GFX::FramebufferHandle> framebuffers(deviceManager->GetBackBufferCount());
	for (int i = 0; i < framebuffers.size(); ++i)
	{
		GFX::FramebufferDesc fbDesc;
		fbDesc.colorAttachments[0] = GFX::FramebufferAttachment(deviceManager->GetBackBufferTexture(i));
		fbDesc.depthAttachment = GFX::FramebufferAttachment(depthTexture.get());

		framebuffers[i] = device->CreateFramebuffer(fbDesc);
	}

	ForwardPass forwardPass;
	forwardPass.Init(device, framebuffers[0].get());

	GFX::View view(70.0f, (float)deviceInfo.backBufferWidth / (float)deviceInfo.backBufferHeight, 0.1f, 100.0);
	view.SetViewport(GFX::Viewport(0, 0, deviceInfo.backBufferWidth, deviceInfo.backBufferHeight));
	view.SetScissor(GFX::Rect(0, 0, deviceInfo.backBufferWidth, deviceInfo.backBufferHeight));
	view.SetPosition(glm::vec3(0.0f, 2.0f, 2.0f));
	view.LookAt(glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0, 1, 0));

	SamplerHandle sampler;
	TextureHandle texture;
	TextureHandle texture1;
	TextureHandle whiteTexture;
	GFX::SamplerDesc samplerDesc;
	sampler = device->CreateSampler(samplerDesc);

	//Texture is temp till we pass in materials to MeshSet
	GFX::Bitmap bitmap;
	bitmap.Load("data/textures/uvTest.png"); 

	GFX::TextureDesc textureDesc;
	textureDesc.width = bitmap.Width();
	textureDesc.height = bitmap.Height();
	textureDesc.format = bitmap.GetFormat();
	textureDesc.name = L"UV Test texture";
	textureDesc.initialState = (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::PIXEL_SHADER_RESOURCE;
	texture = device->CreateTexture(textureDesc);
	device->ImmediateSubmit([=](GFX::ICommandList* cmd)
		{
			cmd->WriteTexture(texture.get(), bitmap);
		});

	bitmap.Load("data/textures/container.png");
	textureDesc.width = bitmap.Width();
	textureDesc.height = bitmap.Height();
	textureDesc.format = bitmap.GetFormat();
	textureDesc.name = L"Box texture";
	texture1 = device->CreateTexture(textureDesc);
	device->ImmediateSubmit([=](GFX::ICommandList* cmd)
		{
			cmd->WriteTexture(texture1.get(), bitmap);
		});

	bitmap.Create(1, 1, Color(1.0f));
	textureDesc.width = bitmap.Width();
	textureDesc.height = bitmap.Height();
	textureDesc.format = bitmap.GetFormat();
	textureDesc.name = L"White texture";
	whiteTexture = device->CreateTexture(textureDesc);
	device->ImmediateSubmit([=](GFX::ICommandList* cmd)
		{
			cmd->WriteTexture(whiteTexture.get(), bitmap);
		});

	GFX::MaterialLibrary materialLib(device);

	GFX::MaterialHandle sphereMaterial = materialLib.CreateMaterial<MaterialData>("SphereMaterial");
	sphereMaterial->RegisterData("diffuseColour", &MaterialData::diffuseColour);
	sphereMaterial->RegisterTexture("diffuseTexture", 0);
	sphereMaterial->SetData("diffuseColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	sphereMaterial->SetData("diffuseTexture", texture, sampler);
	sphereMaterial->UpdateDescriptorSet(device); 

	GFX::MaterialHandle cubeMaterial = materialLib.CreateMaterial<MaterialData>("CubeMaterial");
	cubeMaterial->SetObject<MaterialData>(device);
	cubeMaterial->RegisterData("diffuseColour", &MaterialData::diffuseColour);
	cubeMaterial->RegisterTexture("diffuseTexture", 0);
	cubeMaterial->SetData("diffuseColour", glm::vec4(1.0f, 0.23f, 0.03f, 1.0f));
	cubeMaterial->SetData("diffuseTexture", texture1, sampler);
	cubeMaterial->UpdateDescriptorSet(device);

	GFX::MaterialHandle whiteMaterial = materialLib.CreateMaterial<MaterialData>("WhiteMaterial");
	whiteMaterial->SetObject<MaterialData>(device);
	whiteMaterial->RegisterData("diffuseColour", &MaterialData::diffuseColour);
	whiteMaterial->RegisterTexture("diffuseTexture", 0);
	whiteMaterial->SetData("diffuseTexture", whiteTexture, sampler);
	whiteMaterial->UpdateDescriptorSet(device);

	const MaterialData& test = sphereMaterial->As<MaterialData>();

	GFX::GLTFLoader gltfLoader;
	std::vector<MeshInfo> gltfMeshInfo;
	std::vector<MeshInstance> gltfMeshInstance;
	gltfLoader.Load("data/models/Suzanne.gltf", device, materialLib, gltfMeshInfo, gltfMeshInstance);

	MeshInfo sphereMeshInfo;
	sphereMeshInfo.name = L"SphereMesh";
	GFX::MeshGenerator::PackMesh(GFX::MeshGenerator::UVSphere(), device, sphereMeshInfo, true, true, false);
	sphereMeshInfo.indexOffset = 0;
	sphereMeshInfo.numVertices = sphereMeshInfo.buffer.vertexBuffer->GetDesc().VertexCount();
	sphereMeshInfo.numIndices = sphereMeshInfo.buffer.indexBuffer->GetDesc().IndexCount();

	MeshInfo cubeMeshInfo;
	cubeMeshInfo.name = L"CubeMesh";
	GFX::MeshGenerator::PackMesh(GFX::MeshGenerator::CubePrimative(), device, cubeMeshInfo, true, true, false);
	cubeMeshInfo.indexOffset = 0;
	cubeMeshInfo.numVertices = cubeMeshInfo.buffer.vertexBuffer->GetDesc().VertexCount();
	cubeMeshInfo.numIndices = cubeMeshInfo.buffer.indexBuffer->GetDesc().IndexCount();

	MeshInfo quadMeshInfo;
	quadMeshInfo.name = L"quad";
	GFX::MeshGenerator::PackMesh(GFX::MeshGenerator::QuadPrimative(), device, quadMeshInfo, true, true, false);
	quadMeshInfo.indexOffset = 0;
	quadMeshInfo.numVertices = quadMeshInfo.buffer.vertexBuffer->GetDesc().VertexCount();
	quadMeshInfo.numIndices = quadMeshInfo.buffer.indexBuffer->GetDesc().IndexCount();

	MeshInstance sphereMesh;
	sphereMesh.mesh = &sphereMeshInfo;
	sphereMesh.instanceOffset = 0;
	sphereMesh.material = sphereMaterial.get();

	MeshInstance monkeyMesh = gltfMeshInstance[0];

	MeshInstance planeMesh;
	planeMesh.mesh = &quadMeshInfo;
	planeMesh.instanceOffset = 0;
	planeMesh.material = whiteMaterial.get();
	planeMesh.transform.SetScale(glm::vec3(100, 100, 100));
	planeMesh.transform.SetPosition(glm::vec3(0, -1, 0));
	planeMesh.transform.Rotate(glm::vec3(1, 0, 0), glm::radians(-90.0f));
	planeMesh.transform.UpdateTransform();

	std::array<MeshInstance*, 3> meshes{&sphereMesh, &monkeyMesh, &planeMesh};

	typedef std::chrono::steady_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;
	auto previousTime = clock::now();

	float elapsedTime = 0;
	while (!shouldClose)
	{
		window->PollEvents();
		input.Update();

		auto now = clock::now();
		float deltaTime = std::chrono::duration_cast<duration>(now - previousTime).count() / 1000.0f;
		previousTime = now;
		elapsedTime += deltaTime;

		INF::Log::Info(std::format("DeltaTime={0}", deltaTime));

		//move camera with WASD
		glm::vec3 front = view.Front();
		glm::vec3 up = view.Up();
		glm::vec3 right = view.Right();
		static float moveSpeed = 3.0f;
		if (input.IsKeyDown(KeyCode::A))
		{
			view.Translate(right * moveSpeed * deltaTime);
		}
		if (input.IsKeyDown(KeyCode::D))
		{
			view.Translate(-right * moveSpeed * deltaTime);
		}
		if (input.IsKeyDown(KeyCode::W))
		{
			view.Translate(front * moveSpeed * deltaTime);
		}
		if (input.IsKeyDown(KeyCode::S))
		{
			view.Translate(-front * moveSpeed * deltaTime);
		}
		if (input.IsKeyDown(KeyCode::Space))
		{
			view.Translate(-up * moveSpeed * deltaTime);
		}
		if (input.IsKeyDown(KeyCode::LeftControl))
		{
			view.Translate(up * moveSpeed * deltaTime);
		}

		glm::vec3 posOffset = glm::vec3(sinf(elapsedTime), 0.0f, 0.0f);
		//meshInstance.transform.SetPosition(posOffset * 2.0f);
		sphereMesh.transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), moveSpeed * deltaTime);
		sphereMesh.transform.UpdateTransform();

		glm::vec3 pos1 = -posOffset * 2.0f;
		pos1.z -= 2.0f;
		monkeyMesh.transform.SetPosition(pos1);
		monkeyMesh.transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), moveSpeed * deltaTime);
		monkeyMesh.transform.UpdateTransform();

		deviceManager->BeginFrame();
		deviceManager->ImguiNewFrame();

		cmd->Open();

		
		//need better barrier managment
		cmd->ClearDepth(depthTexture.get(), 1.0f, 0);
		cmd->ClearColor(deviceManager->GetCurrentBackBufferTexture(), GFX::Color(0.2f, 0.2f, 0.2f, 1.0f));


		forwardPass.Prepare(cmd.get(), view, meshes.data(), meshes.size());

		auto frame = deviceManager->GetCurrentBackBufferIndex();
		IFramebuffer* backBuffer = framebuffers[frame].get();
		forwardPass.Render(cmd.get(), backBuffer);

		//add imgui to commandlist
		deviceManager->RenderImgui(cmd.get());

		//transition to present for swapchain
		cmd->Close();
		device->ExecuteCommandLists(cmd.get());
		deviceManager->Present();

		//device->WaitForIdle();

		if (input.IsKeyRelease(KeyCode::Escape))
			shouldClose = true;
	}


	Log::Info("Closing editor");

}