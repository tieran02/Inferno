#pragma once
#include "graphics/interface/device.h"
#include "directx/d3dx12.h"
#include <wrl/client.h>
#include <dxgi1_4.h>

namespace INF::GFX
{
	class D3D12Device : public IDevice
	{
	public:
		D3D12Device(DeviceCreationParameters createInfo);

		CommandListeHandle CreateCommandList(CommandQueue queueType) override;
		uint64_t ExecuteCommandLists(const ICommandList* commandLists, uint32_t commandListCount) override;

	private:
		void CreateDebugController();
		void CreateFactory();
		void CreateAdapter();
		void CreateGraphicsCommandAllocator();

		//Device members
		Microsoft::WRL::ComPtr<ID3D12Debug1> m_debugController;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> m_adapter;
		Microsoft::WRL::ComPtr<ID3D12Device> m_device;
		Microsoft::WRL::ComPtr<ID3D12DebugDevice> m_debugDevice;

		//Context members
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_graphicsCommandAllocator;
	};
}