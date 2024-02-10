#pragma once
#include "graphics/graphicDefines.h"
#include "graphics/interface/device.h"

namespace INF { class IWindow; }

namespace INF::GFX
{
	class IDeviceManager;
	using DeviceManagerHandle = std::unique_ptr<IDeviceManager>;

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
		Format swapChainFormat = Format::RGBA8_UNORM;
		bool vsync = true;
		bool enableDebugValidation = false;
	};

	class IDeviceManager
	{
	public:
		static DeviceManagerHandle Create(API api, const DeviceCreationParameters& createInfo);
		virtual bool CreateDeviceAndSwapChain(IWindow* window, const DeviceCreationParameters& createInfo) = 0;

		virtual IDevice* GetDevice() = 0;
		virtual API GetAPI() const = 0;

		virtual uint32_t GetCurrentBackBufferIndex() = 0;
		virtual uint32_t GetBackBufferCount() = 0;

		virtual void Present() = 0;
	};

}