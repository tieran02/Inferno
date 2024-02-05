#pragma once
#include "infPCH.h"
#include "graphics/d3d12/d3d12Device.h"
#include "graphics/d3d12/D3D12Defines.h"
#include "graphics/d3d12/D3D12Shader.h"

namespace INF::GFX
{

	D3D12Device::D3D12Device(DeviceCreationParameters createInfo)
	{
		if(createInfo.enableDebugValidation)
			CreateDebugController();

		CreateFactory();
		CreateAdapter();

		CreateGraphicsCommandAllocator();
	}

	void D3D12Device::CreateDebugController()
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> dc;
		VerifySuccess(D3D12GetDebugInterface(IID_PPV_ARGS(&dc)));

		VerifySuccess(dc->QueryInterface(IID_PPV_ARGS(&m_debugController)));

		m_debugController->EnableDebugLayer();
		m_debugController->SetEnableGPUBasedValidation(true);
	}

	void D3D12Device::CreateFactory()
	{
		UINT dxgiFactoryFlags = 0;
		if (m_debugController)
		{
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		VerifySuccess(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));
	}

	void D3D12Device::CreateAdapter()
	{
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_dxgiFactory->EnumAdapters1(adapterIndex, &m_adapter); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			HRESULT hr = m_adapter->GetDesc1(&desc);
			if (FAILED(hr))
				continue;

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				continue;
			}

			// Check to see if the adapter supports Direct3D 12,
			// but don't create the actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
		INF_ASSERT(m_adapter.Get(), "Failed to create adapter"); //If assert is hit then there was now supported adapteru

		//Now create the device here
		VerifySuccess(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

		if (m_debugController)
		{
			VerifySuccess(m_device->QueryInterface(IID_PPV_ARGS(&m_debugDevice)));
		}
	}

	void D3D12Device::CreateGraphicsCommandAllocator()
	{
		VerifySuccess(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_graphicsCommandAllocator)));

		m_graphicsQueue = std::move(std::unique_ptr<D3D12Queue>(new D3D12Queue(m_device.Get(), CommandQueue::GRAPHICS)));
	}

	CommandListeHandle D3D12Device::CreateCommandList(CommandQueue queueType)
	{
		return CommandListeHandle(new D3D12CommandList(m_device.Get(), m_graphicsCommandAllocator.Get(), queueType));
	}

	uint64_t D3D12Device::ExecuteCommandLists(const ICommandList* commandLists, uint32_t commandListCount)
	{
		return 0;
	}

	void D3D12Device::WaitForIdle()
	{
		//Wait for the graphics queue to finish, if we have more queues later we will need to add them (E.G compute/copy)
		m_graphicsQueue->Wait();
	}

	ShaderHandle D3D12Device::CreateShader(const ShaderDesc& desc)
	{
		return ShaderHandle(new D3D12Shader(desc));
	}

}