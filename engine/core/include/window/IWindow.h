#pragma once
#include <string>
#include <functional>
#include <memory>
#include "input/input.h"

namespace INF 
{

	using WindowResizeCallback = std::function<void(uint32_t width, uint32_t height)>;
	using WindowCloseCallback = std::function<void()>;


	enum class WindowMode
	{
		FULLSCREEN,
		WINDOWED,
	};

	class IWindow
	{
	public:
		static std::unique_ptr<IWindow> Create(std::string_view title, uint32_t width, uint32_t height);
		virtual ~IWindow() = default;

		virtual void SetTitle(std::string_view title) = 0;
		virtual std::string_view GetTitle() const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void SetResizeCallBack(WindowResizeCallback callback) = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void PollEvents() = 0;

		virtual void Close() = 0;
		virtual void SetCloseCallBack(WindowCloseCallback callback) = 0;

		virtual void SetInputKeyRegisterCallback(InputRegisterKeyFn inputRegisterKeyFn) = 0;
		virtual void SetInputMouseButtonRegisterCallback(InputRegisterMousebuttonFn inputRegisterMouseButtonFn) = 0;
	};
}