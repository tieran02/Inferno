#pragma once
#include "imgui.h"

namespace INF
{
	class IWindow;
}

namespace INF::GFX
{
	class IDeviceManager;
	class ICommandList;

	class IImgui
	{
	public:
		virtual ~IImgui() = default;
		virtual void CreateContext(INF::IWindow* window, IDeviceManager* deviceManager) = 0;
		virtual void Shutdown() = 0;

		virtual void NewFrame() = 0;
		virtual void Render(ICommandList* commandList) = 0;
	};
}