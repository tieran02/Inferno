#pragma once
#include "infPCH.h"
#include "graphics/interface/device.h"
#include "d3d12Device.h"

using namespace INF::GFX;

DeviceHandle IDevice::Create(API api, DeviceCreationParameters createInfo)
{
	switch (api)
	{
	case INF::GFX::API::D3D12:
		return std::unique_ptr<D3D12Device>(new D3D12Device(createInfo));
		break;
	default:
		INF_ASSERT(true, "API not supported");
		break;
	}
}
