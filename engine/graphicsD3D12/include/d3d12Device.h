#pragma once
#include "graphics/interface/device.h"

namespace INF::GFX
{
	class D3D12Device : public IDevice
	{
	public:
		D3D12Device(DeviceCreationParameters createInfo);
	};
}