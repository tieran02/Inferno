#pragma once
#include "graphics/interface/deviceManager.h"
#include <wrl/client.h>
#include <dxgi1_4.h>
#include "directx/d3d12.h"
#include "graphics/d3d12/D3D12CommandList.h"
#include "graphics/interface/Texture.h"
#include "graphics/d3d12/D3D12Device.h"

namespace INF::GFX
{

	class D3D12DeviceManager : public IDeviceManager
	{
	public:
		D3D12DeviceManager(const DeviceCreationParameters& createInfo);
		~D3D12DeviceManager();
		bool CreateDeviceAndSwapChain(IWindow* window, const DeviceCreationParameters& createInfo) override;

		void BeginFrame() override;
		void Present() override;

		IDevice* GetDevice() override;
		API GetAPI() const override;
		uint32_t GetCurrentBackBufferIndex() override;
		uint32_t GetBackBufferCount() override;
		ITexture* GetCurrentBackBufferTexture() override;
		ITexture* GetBackBufferTexture(uint32_t backbufferIndex) override;

	private:
		DeviceCreationParameters m_createInfo;

		std::unique_ptr<D3D12Device> m_device;
		D3D12Queue* m_graphicsQueue;

		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapchain;
		std::vector<TextureHandle> m_swapchainTextures;

		Microsoft::WRL::ComPtr<ID3D12Fence> m_frameFence; //fence for present
		std::vector<HANDLE> m_frameFenceEvents;
		UINT64 m_frameCount = 1;

		uint32_t m_currentSwapchainBuffer;
	};
}