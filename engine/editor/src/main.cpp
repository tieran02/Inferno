// main.cpp
#include <iostream>
#include "core/Log.h"
#include <format>
#include "core/Assert.h"
#include "window/IWindow.h"
#include <memory>

using namespace INF;

int main()
{
	Log::Info("Starting editor");
	std::unique_ptr<IWindow> window = IWindow::Create("Hello Window", 1280, 720);

	int shouldClose = false;
	window->SetCloseCallBack([&shouldClose]
	{
		shouldClose = true;
	});

	while (!shouldClose)
	{
		window->PollEvents();
	}
	Log::Info("Closing editor");

}