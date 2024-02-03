#pragma once
#include "infPCH.h"
#include "d3d12Device.h"
#include "core/Assert.h"


namespace INF::GFX
{

	D3D12Device::D3D12Device(DeviceCreationParameters createInfo)
	{
		if(createInfo.enableDebugValidation)
			CreateDebugController();

		CreateFactory();
		CreateAdapter();
	}

	void D3D12Device::CreateDebugController()
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> dc;
		HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&dc));
		INF_ASSERT(SUCCEEDED(hr), "Failed to get D3D12GetDebugInterface");

		hr = dc->QueryInterface(IID_PPV_ARGS(&m_debugController));
		INF_ASSERT(SUCCEEDED(hr), "Failed to query debug controller");

		if (SUCCEEDED(hr))
		{
			m_debugController->EnableDebugLayer();
			m_debugController->SetEnableGPUBasedValidation(true);
		}
	}

	void D3D12Device::CreateFactory()
	{
		UINT dxgiFactoryFlags = 0;
		if (m_debugController)
		{
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory));
		INF_ASSERT(SUCCEEDED(hr), "Failed to CreateDXGIFactory2");
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
		HRESULT hr = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
		INF_ASSERT(m_adapter.Get(), "Failed to create device");

		if (m_debugController)
		{
			hr = m_device->QueryInterface(IID_PPV_ARGS(&m_debugDevice));
		}
	}

}