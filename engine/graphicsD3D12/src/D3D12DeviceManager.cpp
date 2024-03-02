#include "infPCH.h"
#include "graphics/d3d12/D3D12DeviceManager.h"
#include "graphics/d3d12/D3D12Device.h"
#include "graphics/d3d12/D3D12Defines.h"
#include "window/IWindow.h"
#include "graphics/d3d12/D3D12CommandList.h"
#include "graphics/d3d12/D3D12Texture.h"
#include <dxgidebug.h>

using namespace INF::GFX;

D3D12DeviceManager::D3D12DeviceManager(const DeviceCreationParameters& createInfo) : m_createInfo(createInfo), m_currentSwapchainBuffer(0)
{
}


D3D12DeviceManager::~D3D12DeviceManager()
{
	m_device->WaitForIdle();

	//Wait for present frames to finish
	for (auto fenceEvent : m_frameFenceEvents)
	{
		WaitForSingleObject(fenceEvent, INFINITE);
		CloseHandle(fenceEvent);
	}
	m_frameFenceEvents.clear();
	//m_FrameFence->Release();
	m_frameFence = nullptr;

	m_swapchainTextures.clear();

	int ref = m_swapchain->Release();
	m_swapchain = nullptr; //Set to nullptr so swapchain is fully released, that way ReportLiveObjects is more useful and shows any dangling refs 

	m_device.reset();


	Microsoft::WRL::ComPtr<IDXGIDebug1> dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
	{
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}
}


IDevice* D3D12DeviceManager::GetDevice()
{
	return m_device.get();
}

API D3D12DeviceManager::GetAPI() const
{
	return API::D3D12;
}

uint32_t D3D12DeviceManager::GetCurrentBackBufferIndex()
{
	return m_currentSwapchainBuffer;
}

uint32_t D3D12DeviceManager::GetBackBufferCount()
{
	return static_cast<uint32_t>(m_swapchainTextures.size());
}

ITexture* D3D12DeviceManager::GetCurrentBackBufferTexture()
{
	return m_swapchainTextures.at(GetCurrentBackBufferIndex()).get();
}

ITexture* D3D12DeviceManager::GetBackBufferTexture(uint32_t backbufferIndex)
{
	INF_ASSERT(backbufferIndex >= 0 && backbufferIndex < GetBackBufferCount(), "backbuffer index out of range");
	return m_swapchainTextures.at(backbufferIndex).get();
}

bool D3D12DeviceManager::CreateDeviceAndSwapChain(IWindow* window, const DeviceCreationParameters& createInfo)
{
	m_device = std::unique_ptr<D3D12Device>(new D3D12Device(createInfo));
	m_graphicsQueue = m_device->GetGraphicsQueue();

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.BufferCount = createInfo.swapChainBufferCount;
	swapchainDesc.Width = createInfo.backBufferWidth;
	swapchainDesc.Height = createInfo.backBufferHeight;
	swapchainDesc.Format = D3D12Format(createInfo.swapChainFormat);
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.SampleDesc.Count = 1;

	IDXGISwapChain1* swapchain1 = nullptr;
	VerifySuccess(m_device->Factory()->CreateSwapChainForHwnd(m_graphicsQueue->D3D(), (HWND)window->GetNativeHandle(), &swapchainDesc, nullptr, nullptr, &swapchain1));

	HRESULT swapchainSupport = swapchain1->QueryInterface(IID_PPV_ARGS(&m_swapchain));
	VerifySuccess(swapchainSupport);

	if (FAILED(swapchainSupport))
	{
		swapchain1->Release();
		return false;
	}


	m_swapchainTextures.resize(createInfo.swapChainBufferCount);
	for (uint32_t n = 0; n < createInfo.swapChainBufferCount; n++)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		VerifySuccess(m_swapchain->GetBuffer(n, IID_PPV_ARGS(&resource)));

		TextureDesc textureDesc;
		textureDesc.width = swapchainDesc.Width;
		textureDesc.height = swapchainDesc.Height;
		textureDesc.format = createInfo.swapChainFormat;
		textureDesc.name = L"SwapChainBuffer";
		m_swapchainTextures[n] = D3D12Texture::CreateTextureFromResource(m_device.get(), resource, textureDesc);

		//Get the view straightaway as we know this will be used as a render target
		m_swapchainTextures[n]->GetView(ITextureView::ViewType::RENDER_TARGET);

	}

	VerifySuccess(m_device->Device()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_frameFence)));

	for (UINT bufferIndex = 0; bufferIndex < createInfo.swapChainBufferCount; bufferIndex++)
	{
		m_frameFenceEvents.push_back(CreateEvent(NULL, false, true, NULL));
	}

	return true;
}


void D3D12DeviceManager::BeginFrame()
{

	//Make sure the current swapchain buffer is finished its last present
	WaitForSingleObject(m_frameFenceEvents[m_currentSwapchainBuffer], INFINITE);
}

void D3D12DeviceManager::Present()
{
	m_swapchain->Present(m_createInfo.vsync, 0);

	m_frameFence->SetEventOnCompletion(m_frameCount, m_frameFenceEvents[m_currentSwapchainBuffer]);
	m_device->GetGraphicsQueue()->D3D()->Signal(m_frameFence.Get(), m_frameCount);

	m_currentSwapchainBuffer = (m_currentSwapchainBuffer + 1) % GetBackBufferCount();

}

