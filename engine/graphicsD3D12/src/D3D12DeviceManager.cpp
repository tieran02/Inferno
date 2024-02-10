#include "infPCH.h"
#include "graphics/d3d12/D3D12DeviceManager.h"
#include "graphics/d3d12/D3D12Device.h"
#include "graphics/d3d12/D3D12Defines.h"
#include "window/IWindow.h"
#include "graphics/d3d12/D3D12CommandList.h"
#include "graphics/d3d12/D3D12Texture.h"

using namespace INF::GFX;

D3D12DeviceManager::D3D12DeviceManager(const DeviceCreationParameters& createInfo) : m_createInfo(createInfo), m_currentSwapchainBuffer(0)
{
}


D3D12DeviceManager::~D3D12DeviceManager()
{

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

	IDXGISwapChain1* swapchain = nullptr;
	VerifySuccess(m_device->Factory()->CreateSwapChainForHwnd(m_graphicsQueue->D3D(), (HWND)window->GetNativeHandle(), &swapchainDesc, nullptr, nullptr, &swapchain));

	HRESULT swapchainSupport = swapchain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&swapchain);
	VerifySuccess(swapchainSupport);

	if (SUCCEEDED(swapchainSupport))
	{
		m_swapchain = (IDXGISwapChain3*)swapchain;
	}
	else
	{
		swapchain->Release();
		return false;
	}

	m_swapchainTextures.resize(createInfo.swapChainBufferCount);
	for (uint32_t n = 0; n < createInfo.swapChainBufferCount; n++)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		VerifySuccess(swapchain->GetBuffer(n, IID_PPV_ARGS(&resource)));

		TextureDesc textureDesc;
		textureDesc.width = swapchainDesc.Width;
		textureDesc.height = swapchainDesc.Height;
		textureDesc.format = createInfo.swapChainFormat;
		textureDesc.name = "SwapChainBuffer";
		m_swapchainTextures[n] = D3D12Texture::CreateTextureFromResource(m_device.get(), resource, textureDesc);

		//Get the view straightaway as we know this will be used as a render target
		m_swapchainTextures[n]->GetView(ITextureView::ViewType::RENDER_TARGET);

	}

	return true;
}

void D3D12DeviceManager::Present()
{
	m_swapchain->Present(m_createInfo.vsync, 0);
	m_currentSwapchainBuffer = (m_currentSwapchainBuffer + 1) % GetBackBufferCount();
}

