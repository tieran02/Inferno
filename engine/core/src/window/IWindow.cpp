#pragma once
#include "window/IWindow.h"
#include "platform/win64/Win64Window.h"

namespace INF 
{
	std::unique_ptr<IWindow> IWindow::Create(std::string_view title, uint32_t width, uint32_t height)
	{
		return std::make_unique<Win64Window>(title, width, height);
	}

}