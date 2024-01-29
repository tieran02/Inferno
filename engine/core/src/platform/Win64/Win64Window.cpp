#include "platform/Win64/Win64Window.h"
#include "core/Assert.h"
#include <format>

using namespace INF;

Win64Window::Win64Window(std::string_view title, uint32_t width, uint32_t height) :
	m_title(title),
	m_width(width),
	m_height(height),
	m_resizeCallback(nullptr),
	m_closeCallback(nullptr)
{
	int result = glfwInit();
	INF_ASSERT(result, std::format("Failed to init GLFW: code={}", result));

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_window = glfwCreateWindow(width, height, title.data(), NULL, NULL);
	INF_ASSERT(m_window, "Failed to create glfw Window");
}

Win64Window::~Win64Window()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}


void Win64Window::SetTitle(std::string_view title)
{
	m_title = title;
}

std::string_view Win64Window::GetTitle() const
{
	return m_title;
}

void Win64Window::Resize(uint32_t width, uint32_t height)
{
	m_width = width;
	m_height = height;

	if(m_resizeCallback)
		m_resizeCallback(width, height);
}

void Win64Window::SetResizeCallBack(WindowResizeCallback callback)
{
	m_resizeCallback = callback;
}

uint32_t Win64Window::GetWidth() const
{
	return m_width;
}

uint32_t Win64Window::GetHeight() const
{
	return m_height;
}

void Win64Window::PollEvents()
{
	glfwPollEvents();

	if (glfwWindowShouldClose(m_window))
		Close();
}

void Win64Window::Close()
{
	if (m_closeCallback)
		m_closeCallback();
}

void Win64Window::SetCloseCallBack(WindowCloseCallback callback)
{
	m_closeCallback = callback;
}

