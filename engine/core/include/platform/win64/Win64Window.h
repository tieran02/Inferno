#pragma once
#include "GLFW/glfw3.h"
#include "window/IWindow.h"

class GLFWwindow;

namespace INF 
{
	class Win64Window : public IWindow
	{
	public:
		Win64Window(std::string_view title, uint32_t width, uint32_t height);
		~Win64Window();
		void SetTitle(std::string_view title) override;
		std::string_view GetTitle() const override;

		void Resize(uint32_t width, uint32_t height) override;
		void SetResizeCallBack(WindowResizeCallback callback) override;
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		void PollEvents() override;
		void Close() override;
		void SetCloseCallBack(WindowCloseCallback callback) override;

		void SetInputKeyRegisterCallback(InputRegisterKeyFn inputRegisterKeyFn) override;
		void SetInputMouseButtonRegisterCallback(InputRegisterMousebuttonFn inputRegisterMouseButtonFn) override;
		void SetInputMouseCursorRegisterCallback(InputRegisterMouseCursorFn inputRegisterMouseCursorFn) override;

		void* GetNativeHandle() override;
		GLFWwindow* GLFWWindow() { return m_window; }

	private:
		std::string m_title;
		uint32_t m_width, m_height;
		WindowResizeCallback m_resizeCallback;
		WindowCloseCallback m_closeCallback;

		InputRegisterKeyFn m_inputRegisterKeyFn;
		InputRegisterMousebuttonFn m_inputRegisterMouseButtonFn;
		InputRegisterMouseCursorFn m_inputRegisterMouseCursorFn;

		GLFWwindow* m_window;
	};
}