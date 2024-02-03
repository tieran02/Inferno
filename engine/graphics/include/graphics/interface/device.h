#pragma once
#include <cstdint>
#include "graphics/graphicDefines.h"
#include <memory>
#include <format>
#include "core/Assert.h"

namespace INF::GFX
{
	struct DeviceCreationParameters
	{
		bool startMaximized = false;
		bool startFullscreen = false;
		bool allowModeSwitch = true;
		int windowPosX = -1;            // -1 means use default placement
		int windowPosY = -1;
		uint32_t backBufferWidth = 1280;
		uint32_t backBufferHeight = 720;
		uint32_t swapChainBufferCount = 3;
		Format swapChainFormat = Format::SRGBA8_UNORM;
		bool enableDebugValidation = false;
	};

	class IDevice;
	using DeviceHandle = std::unique_ptr<IDevice>;

	class IDevice
	{
	public:
		static DeviceHandle Create(API api, DeviceCreationParameters createInfo);
	};
}