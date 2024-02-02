#pragma once
#include <functional>
#include <array>

namespace INF
{
	enum class KeyAction : uint8_t
	{
		RELEASE,
		PRESS,
		REPEAT,
		NONE
	};

	enum class KeyModifier : uint8_t
	{
		SHIFT = 1 << 0,
		CONTROL = 1 << 1,
		ALT = 1 << 2,
		WINDOWS = 1 << 3,
		CAPS_LOCK = 1 << 4,
		NUM_LOCK = 1 << 5,
	};
	inline constexpr KeyModifier operator|(KeyModifier Lhs, KeyModifier Rhs)
	{
		return static_cast<KeyModifier>(static_cast<std::underlying_type_t<KeyModifier>>(Lhs) |
			static_cast<std::underlying_type_t<KeyModifier>>(Rhs));
	}

	enum class MouseButton : uint8_t
	{
		Left,
		Right,
		Middle,
		COUNT
	};

	enum class KeyCode : uint8_t
	{
		Unkown = 0x00,
		Backspace = 0x08,
		Tab = 0x09,
		Clear = 0x0C,
		Enter = 0x0D,
		Shift = 0x10,
		Control = 0x11,
		Alt = 0x12,
		Pause = 0x13,
		CapsLock = 0x14,
		Escape = 0x1B,
		Space = 0x20,
		PageUp = 0x21,
		PageDown = 0x22,
		End = 0x23,
		Home = 0x24,
		Left = 0x25,
		Up = 0x26,
		Right = 0x27,
		Down = 0x28,
		Select = 0x29,
		Print = 0x2A,
		Execute = 0x2B,
		PrintScreen = 0x2C,
		Insert = 0x2D,
		Delete = 0x2E,
		Help = 0x2F,
		Zero = 0x30,
		One = 0x31,
		Two = 0x32,
		Three = 0x33,
		Four = 0x34,
		Five = 0x35,
		Six = 0x36,
		Seven = 0x37,
		Eight = 0x38,
		Nine = 0x39,
		A = 0x41,
		B = 0x42,
		C = 0x43,
		D = 0x44,
		E = 0x45,
		F = 0x46,
		G = 0x47,
		H = 0x48,
		I = 0x49,
		J = 0x4A,
		K = 0x4B,
		L = 0x4C,
		M = 0x4D,
		N = 0x4E,
		O = 0x4F,
		P = 0x50,
		Q = 0x51,
		R = 0x52,
		S = 0x53,
		T = 0x54,
		U = 0x55,
		V = 0x56,
		W = 0x57,
		X = 0x58,
		Y = 0x59,
		Z = 0x5A,
		LeftWindowsKey = 0x5B,
		RightWindowsKey = 0x5C,
		ApplicationsKey = 0x5D,
		Sleep = 0x5F,
		NumPad0 = 0x60,
		NumPad1 = 0x61,
		NumPad2 = 0x62,
		NumPad3 = 0x63,
		NumPad4 = 0x64,
		NumPad5 = 0x65,
		NumPad6 = 0x66,
		NumPad7 = 0x67,
		NumPad8 = 0x68,
		NumPad9 = 0x69,
		Multiply = 0x6A,
		Add = 0x6B,
		Seperator = 0x6C,
		Subtract = 0x6D,
		Decimal = 0x6E,
		Divide = 0x6F,
		F1 = 0x70,
		F2 = 0x71,
		F3 = 0x72,
		F4 = 0x73,
		F5 = 0x74,
		F6 = 0x75,
		F7 = 0x76,
		F8 = 0x77,
		F9 = 0x78,
		F10 = 0x79,
		F11 = 0x7A,
		F12 = 0x7B,
		F13 = 0x7C,
		F14 = 0x7D,
		F15 = 0x7E,
		F16 = 0x7F,
		F17 = 0x80,
		F18 = 0x81,
		F19 = 0x82,
		F20 = 0x83,
		F21 = 0x84,
		F22 = 0x85,
		F23 = 0x86,
		F24 = 0x87,
		Numlock = 0x90,
		ScrollLock = 0x91,
		LeftShift = 0xA0,
		RightShift = 0xA1,
		LeftControl = 0xA2,
		RightContol = 0xA3,
		LeftMenu = 0xA4,
		RightMenu = 0xA5,
		BrowserBack = 0xA6,
		BrowserForward = 0xA7,
		BrowserRefresh = 0xA8,
		BrowserStop = 0xA9,
		BrowserSearch = 0xAA,
		BrowserFavorites = 0xAB,
		BrowserHome = 0xAC,
		VolumeMute = 0xAD,
		VolumeDown = 0xAE,
		VolumeUp = 0xAF,
		NextTrack = 0xB0,
		PreviousTrack = 0xB1,
		StopMedia = 0xB2,
		PlayPause = 0xB3,
		LaunchMail = 0xB4,
		SelectMedia = 0xB5,
		LaunchApp1 = 0xB6,
		LaunchApp2 = 0xB7,
		OEM1 = 0xBA,
		OEMPlus = 0xB8,
		OEMComma = 0xBC,
		OEMMinus = 0xBD,
		OEMPeriod = 0xBE,
		OEM2 = 0xBF,
		OEM3 = 0xC0,
		OEM4 = 0xDB,
		OEM5 = 0xDC,
		OEM6 = 0xDD,
		OEM7 = 0xDE,
		OEM8 = 0xDF,
		OEM102 = 0xE2,
		Process = 0xE5,
		Packet = 0xE7,
		Attn = 0xF6,
		CrSel = 0xF7,
		ExSel = 0xF8,
		EraseEOF = 0xF9,
		Play = 0xFA,
		Zoom = 0xFB,
		PA1 = 0xFD,
		OEMClear = 0xFE,
		COUNT
	};

	using InputRegisterKeyFn = std::function<void(KeyCode key, int scancode, INF::KeyAction action, int mods)>;
	using InputRegisterMousebuttonFn = std::function<void(MouseButton button,INF::KeyAction action, int mods)>;
	using InputRegisterMouseCursorFn = std::function<void(double xPos, double yPos)>;


	class Input
	{
	public:
		Input();

		void Update();

		bool IsKeyPress(KeyCode key) const;
		bool IsKeyRelease(KeyCode key) const;
		bool IsKeyDown(KeyCode key) const;

		bool IsMouseButtonPress(MouseButton button) const;
		bool IsMouseButtonRelease(MouseButton button) const;
		bool IsMouseButtonDown(MouseButton button) const;

		double GetMouseX() const;
		double GetMouseY() const;
		double GetDeltaMouseX() const;
		double GetDeltaMouseY() const;

		inline InputRegisterKeyFn GetRegisterKeyFn()
		{
			return std::bind(&Input::RegisterKey, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		}

		inline InputRegisterMousebuttonFn GetRegisterMouseButtonFn()
		{
			return std::bind(&Input::RegisterMouseButton, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		}

		inline InputRegisterMouseCursorFn GetRegisterMouseCursorFn()
		{
			return std::bind(&Input::RegisterMouseCursorButton, this, std::placeholders::_1, std::placeholders::_2);
		}

	private:
		void RegisterKey(KeyCode key, int scancode, KeyAction action, int mods);
		void RegisterMouseButton(MouseButton button, KeyAction action, int mods);
		void RegisterMouseCursorButton(double xPos, double yPos);
		
		std::array<KeyAction, std::underlying_type_t<KeyCode>(KeyCode::COUNT)> m_keyStates;
		std::array<KeyAction, std::underlying_type_t<KeyCode>(KeyCode::COUNT)> m_previousKeyStates;

		std::array<KeyAction, std::underlying_type_t<KeyCode>(MouseButton::COUNT)> m_mouseButtonStates;
		std::array<KeyAction, std::underlying_type_t<KeyCode>(MouseButton::COUNT)> m_previousMouseButtonStates;

		double m_mousePosX;
		double m_mousePosY;
		double m_callbackMousePosX;
		double m_callbackMousePosY;
		double m_prevMousePosX;
		double m_prevMousePosY;
	};
}