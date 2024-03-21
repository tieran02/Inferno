#include "infPCH.h"
#include "graphics/d3d12/D3D12Imgui.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_dx12.h"

#include "platform/win64/Win64Window.h"
#include "graphics/d3d12/D3D12DeviceManager.h"
#include "graphics/d3d12/D3D12Device.h"
#include "GLFW/glfw3.h"
#include "graphics/d3d12/D3D12Defines.h"
#include "graphics/d3d12/D3D12CommandList.h"

using namespace INF::GFX;

D3D12Imgui::D3D12Imgui()
{

}

D3D12Imgui::~D3D12Imgui()
{

}


void D3D12Imgui::Shutdown()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	m_device->SRVDescriptoHeap().ReleaseDescriptor(m_descriptorIndex);
}


void D3D12Imgui::CreateContext(INF::IWindow* window, IDeviceManager* deviceManager)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	GLFWwindow* glfwWindow = static_cast<Win64Window*>(window)->GLFWWindow();
	ImGui_ImplGlfw_InitForOther(glfwWindow, true);

	D3D12DeviceManager* d3D12DeviceManager = static_cast<D3D12DeviceManager*>(deviceManager);
	m_device = static_cast<D3D12Device*>(d3D12DeviceManager->GetDevice());
	m_descriptorIndex = m_device->SRVDescriptoHeap().AllocateDescriptor();


	ID3D12Device* device = m_device->Device();
	int num_frames_in_flight = d3D12DeviceManager->GetBackBufferCount();
	DXGI_FORMAT rtv_format = D3D12Format(d3D12DeviceManager->GetBackBufferTexture(0)->GetDesc().format);
	ID3D12DescriptorHeap* cbv_srv_heap = m_device->SRVDescriptoHeap().Heap();
	D3D12_CPU_DESCRIPTOR_HANDLE font_srv_cpu_desc_handle = m_device->SRVDescriptoHeap().GetCPUHandle(m_descriptorIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE font_srv_gpu_desc_handle = m_device->SRVDescriptoHeap().GetGPUHandle(m_descriptorIndex);

	ImGui_ImplDX12_Init(device, num_frames_in_flight, rtv_format, cbv_srv_heap, font_srv_cpu_desc_handle, font_srv_gpu_desc_handle);
}

void D3D12Imgui::NewFrame()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void D3D12Imgui::Render(ICommandList* commandList)
{
	D3D12CommandList* cmd = static_cast<D3D12CommandList*>(commandList);

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmd->D3D());
}
