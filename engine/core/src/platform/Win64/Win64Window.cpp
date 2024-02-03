#include "infPCH.h"
#include "platform/Win64/Win64Window.h"
#include "core/Assert.h"

using namespace INF;

extern std::unordered_map<int, KeyCode> g_glfwKeyCodeMap;
extern std::unordered_map<int, MouseButton> g_glfwMouseButtonCodeMap;

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

	glfwSetWindowUserPointer(m_window, static_cast<void*>(this));
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

void Win64Window::SetInputKeyRegisterCallback(InputRegisterKeyFn inputRegisterKeyFn)
{
	m_inputRegisterKeyFn = inputRegisterKeyFn;
	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		INF_ASSERT(g_glfwKeyCodeMap.find(key) != g_glfwKeyCodeMap.end(), std::format("Key:{} not found in map", key));
		KeyCode keyCode = g_glfwKeyCodeMap.at(key);
		Win64Window* self = static_cast<Win64Window*>(glfwGetWindowUserPointer(window));
		self->m_inputRegisterKeyFn(keyCode,
			scancode,
			static_cast<KeyAction>(action),
			mods);
	});
}

void Win64Window::SetInputMouseButtonRegisterCallback(InputRegisterMousebuttonFn inputRegisterMouseButtonFn)
{
	m_inputRegisterMouseButtonFn = inputRegisterMouseButtonFn;
	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int key, int action, int mods)
		{
			INF_ASSERT(g_glfwMouseButtonCodeMap.find(key) != g_glfwMouseButtonCodeMap.end(), std::format("Key:{} not found in map", key));
			MouseButton button = g_glfwMouseButtonCodeMap.at(key);
			Win64Window* self = static_cast<Win64Window*>(glfwGetWindowUserPointer(window));
			self->m_inputRegisterMouseButtonFn(button,
				static_cast<KeyAction>(action),
				mods);
		});
}

void Win64Window::SetInputMouseCursorRegisterCallback(InputRegisterMouseCursorFn inputRegisterMouseCursorFn)
{
	m_inputRegisterMouseCursorFn = inputRegisterMouseCursorFn;
	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
		{
			Win64Window* self = static_cast<Win64Window*>(glfwGetWindowUserPointer(window));
			self->m_inputRegisterMouseCursorFn(xPos, yPos);
		});
}

std::unordered_map<int, KeyCode> g_glfwKeyCodeMap
{
	{GLFW_KEY_SPACE              , KeyCode::Space},
	{GLFW_KEY_APOSTROPHE         , KeyCode::OEM3},
	{GLFW_KEY_COMMA              , KeyCode::OEMComma},
	{GLFW_KEY_MINUS              , KeyCode::OEMMinus},
	{GLFW_KEY_PERIOD             , KeyCode::OEMPeriod},
	{GLFW_KEY_SLASH              , KeyCode::OEM2},
	{GLFW_KEY_0                  , KeyCode::Zero},
	{GLFW_KEY_1                  , KeyCode::One},
	{GLFW_KEY_2                  , KeyCode::Two},
	{GLFW_KEY_3                  , KeyCode::Three},
	{GLFW_KEY_4                  , KeyCode::Four},
	{GLFW_KEY_5                  , KeyCode::Five},
	{GLFW_KEY_6                  , KeyCode::Six},
	{GLFW_KEY_7                  , KeyCode::Seven},
	{GLFW_KEY_8                  , KeyCode::Eight},
	{GLFW_KEY_9                  , KeyCode::Nine},
	{GLFW_KEY_SEMICOLON          , KeyCode::OEM1},
	{GLFW_KEY_EQUAL              , KeyCode::OEMPlus},
	{GLFW_KEY_A                  , KeyCode::A},
	{GLFW_KEY_B                  , KeyCode::B},
	{GLFW_KEY_C                  , KeyCode::C},
	{GLFW_KEY_D                  , KeyCode::D},
	{GLFW_KEY_E                  , KeyCode::E},
	{GLFW_KEY_F                  , KeyCode::F},
	{GLFW_KEY_G                  , KeyCode::G},
	{GLFW_KEY_H                  , KeyCode::H},
	{GLFW_KEY_I                  , KeyCode::I},
	{GLFW_KEY_J                  , KeyCode::J},
	{GLFW_KEY_K                  , KeyCode::K},
	{GLFW_KEY_L                  , KeyCode::L},
	{GLFW_KEY_M                  , KeyCode::M},
	{GLFW_KEY_N                  , KeyCode::N},
	{GLFW_KEY_O                  , KeyCode::O},
	{GLFW_KEY_P                  , KeyCode::P},
	{GLFW_KEY_Q                  , KeyCode::Q},
	{GLFW_KEY_R                  , KeyCode::R},
	{GLFW_KEY_S                  , KeyCode::S},
	{GLFW_KEY_T                  , KeyCode::T},
	{GLFW_KEY_U                  , KeyCode::U},
	{GLFW_KEY_V                  , KeyCode::V},
	{GLFW_KEY_W                  , KeyCode::W},
	{GLFW_KEY_X                  , KeyCode::X},
	{GLFW_KEY_Y                  , KeyCode::Y},
	{GLFW_KEY_Z                  , KeyCode::Z},
	{GLFW_KEY_LEFT_BRACKET       , KeyCode::OEM4},
	{GLFW_KEY_BACKSLASH          , KeyCode::OEM5},
	{GLFW_KEY_RIGHT_BRACKET      , KeyCode::OEM6},
	{GLFW_KEY_GRAVE_ACCENT       , KeyCode::OEM8},
	{GLFW_KEY_WORLD_1            , KeyCode::LeftWindowsKey},
	{GLFW_KEY_WORLD_2            , KeyCode::RightWindowsKey},
	//FUNCTION KEYS

	{GLFW_KEY_ESCAPE             , KeyCode::Escape},
	{GLFW_KEY_ENTER              , KeyCode::Enter},
	{GLFW_KEY_TAB                , KeyCode::Tab},
	{GLFW_KEY_BACKSPACE          , KeyCode::Backspace},
	{GLFW_KEY_INSERT             , KeyCode::Insert},
	{GLFW_KEY_DELETE             , KeyCode::Delete},
	{GLFW_KEY_RIGHT              , KeyCode::Right},
	{GLFW_KEY_LEFT               , KeyCode::Left},
	{GLFW_KEY_DOWN               , KeyCode::Down},
	{GLFW_KEY_UP                 , KeyCode::Up},
	{GLFW_KEY_PAGE_UP            , KeyCode::PageUp},
	{GLFW_KEY_PAGE_DOWN          , KeyCode::PageDown},
	{GLFW_KEY_HOME               , KeyCode::Home},
	{GLFW_KEY_END                , KeyCode::End},
	{GLFW_KEY_CAPS_LOCK          , KeyCode::CapsLock},
	{GLFW_KEY_SCROLL_LOCK        , KeyCode::ScrollLock},
	{GLFW_KEY_NUM_LOCK           , KeyCode::Numlock},
	{GLFW_KEY_PRINT_SCREEN       , KeyCode::PrintScreen},
	{GLFW_KEY_PAUSE              , KeyCode::Pause},
	{GLFW_KEY_F1                 , KeyCode::F1},
	{GLFW_KEY_F2                 , KeyCode::F2},
	{GLFW_KEY_F3                 , KeyCode::F3},
	{GLFW_KEY_F4                 , KeyCode::F4},
	{GLFW_KEY_F5                 , KeyCode::F5},
	{GLFW_KEY_F6                 , KeyCode::F6},
	{GLFW_KEY_F7                 , KeyCode::F7},
	{GLFW_KEY_F8                 , KeyCode::F8},
	{GLFW_KEY_F9                 , KeyCode::F9},
	{GLFW_KEY_F10                , KeyCode::F10},
	{GLFW_KEY_F11                , KeyCode::F11},
	{GLFW_KEY_F12                , KeyCode::F12},
	{GLFW_KEY_F13                , KeyCode::F13},
	{GLFW_KEY_F14                , KeyCode::F14},
	{GLFW_KEY_F15                , KeyCode::F15},
	{GLFW_KEY_F16                , KeyCode::F16},
	{GLFW_KEY_F17                , KeyCode::F17},
	{GLFW_KEY_F18                , KeyCode::F18},
	{GLFW_KEY_F19                , KeyCode::F19},
	{GLFW_KEY_F20                , KeyCode::F20},
	{GLFW_KEY_F21                , KeyCode::F21},
	{GLFW_KEY_F22                , KeyCode::F22},
	{GLFW_KEY_F23                , KeyCode::F23},
	{GLFW_KEY_F24                , KeyCode::F24},
	{GLFW_KEY_F25                , KeyCode::F24},
	{GLFW_KEY_KP_0               , KeyCode::NumPad0},
	{GLFW_KEY_KP_1               , KeyCode::NumPad1},
	{GLFW_KEY_KP_2               , KeyCode::NumPad2},
	{GLFW_KEY_KP_3               , KeyCode::NumPad3},
	{GLFW_KEY_KP_4               , KeyCode::NumPad4},
	{GLFW_KEY_KP_5               , KeyCode::NumPad5},
	{GLFW_KEY_KP_6               , KeyCode::NumPad6},
	{GLFW_KEY_KP_7               , KeyCode::NumPad7},
	{GLFW_KEY_KP_8               , KeyCode::NumPad8},
	{GLFW_KEY_KP_9               , KeyCode::NumPad9},
	{GLFW_KEY_KP_DECIMAL         , KeyCode::Delete},
	{GLFW_KEY_KP_DIVIDE          , KeyCode::Divide},
	{GLFW_KEY_KP_MULTIPLY        , KeyCode::Multiply},
	{GLFW_KEY_KP_SUBTRACT        , KeyCode::Subtract},
	{GLFW_KEY_KP_ADD             , KeyCode::Add},
	{GLFW_KEY_KP_ENTER           , KeyCode::Enter},
	{GLFW_KEY_KP_EQUAL           , KeyCode::OEMPlus},
	{GLFW_KEY_LEFT_SHIFT         , KeyCode::LeftShift},
	{GLFW_KEY_LEFT_CONTROL       , KeyCode::LeftControl},
	{GLFW_KEY_LEFT_ALT           , KeyCode::LeftMenu},
	{GLFW_KEY_LEFT_SUPER         , KeyCode::LeftWindowsKey},
	{GLFW_KEY_RIGHT_SHIFT        , KeyCode::RightShift},
	{GLFW_KEY_RIGHT_CONTROL      , KeyCode::RightContol},
	{GLFW_KEY_RIGHT_ALT          , KeyCode::RightMenu},
	{GLFW_KEY_RIGHT_SUPER        , KeyCode::RightWindowsKey},
	{GLFW_KEY_MENU               , KeyCode::Alt},
};

std::unordered_map<int, MouseButton> g_glfwMouseButtonCodeMap
{
	{GLFW_MOUSE_BUTTON_LEFT      , MouseButton::Left},
	{GLFW_MOUSE_BUTTON_RIGHT     , MouseButton::Right},
	{GLFW_MOUSE_BUTTON_MIDDLE    , MouseButton::Middle},
};