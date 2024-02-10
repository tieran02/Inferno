#include "infPCH.h"
#include "graphics/interface/deviceManager.h"
#include "graphics/d3d12/D3D12DeviceManager.h"

INF::GFX::DeviceManagerHandle INF::GFX::IDeviceManager::Create(API api, const DeviceCreationParameters& createInfo)
{
	switch (api)
	{
	case INF::GFX::API::D3D12:
		return std::unique_ptr<IDeviceManager>(new D3D12DeviceManager(createInfo));
	default:
		INF_ASSERT(false, "API not supported");
		return nullptr;
	}
}

