// main.cpp
#include <iostream>
#include "core/Log.h"
#include <format>
#include "core/Assert.h"
#include "window/IWindow.h"
#include <memory>
#include <functional>

using namespace INF;

int main()
{
	Log::Info("Starting editor");
	std::unique_ptr<IWindow> window = IWindow::Create("Hello Window", 1280, 720);
	Input input;
	window->SetInputKeyRegisterCallback(input.GetRegisterKeyFn());
	window->SetInputMouseButtonRegisterCallback(input.GetRegisterMouseButtonFn());
	window->SetInputMouseCursorRegisterCallback(input.GetRegisterMouseCursorFn());

	int shouldClose = false;
	window->SetCloseCallBack([&shouldClose]
	{
		shouldClose = true;
	});

	while (!shouldClose)
	{
		window->PollEvents();
		input.Update();

		if (input.IsKeyPress(KeyCode::A))
			Log::Info("A key pressed");
		if (input.IsKeyRelease(KeyCode::A))
			Log::Info("A key release");

		if (input.IsMouseButtonPress(MouseButton::Left))
			Log::Info("Left Click");
		if (input.IsMouseButtonPress(MouseButton::Right))
			Log::Info("Right Click");
		if (input.IsMouseButtonPress(MouseButton::Middle))
			Log::Info("Middle Click");

		//Log::Info(std::format("MouseX={} MouseY={}", input.GetMouseX(), input.GetMouseY()));
		Log::Info(std::format("DeltaX={:10f} DeltaY={:10f}", input.GetDeltaMouseX(), input.GetDeltaMouseY()));


		if (input.IsKeyRelease(KeyCode::Escape))
			shouldClose = true;
	}
	Log::Info("Closing editor");

}