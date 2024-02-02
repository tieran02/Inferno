#include "input/input.h"
#include "core/Log.h"
#include <format>

using namespace INF;

Input::Input()
{
	std::fill(m_keyStates.begin(), m_keyStates.end(), KeyAction::NONE);
	std::fill(m_previousKeyStates.begin(), m_previousKeyStates.end(), KeyAction::NONE);
}

void Input::RegisterKey(KeyCode key, int scancode, KeyAction action, int mods)
{
	if(action == KeyAction::PRESS)
		m_keyStates[static_cast<std::underlying_type_t<KeyCode>>(key)] = action;
}

void Input::RegisterMouseButton(MouseButton button, KeyAction action, int mods)
{
	if (action == KeyAction::PRESS)
		m_mouseButtonStates[static_cast<std::underlying_type_t<MouseButton>>(button)] = action;
}

bool Input::IsKeyPress(KeyCode key) const
{
	return m_keyStates.at(static_cast<std::underlying_type_t<KeyCode>>(key)) == KeyAction::PRESS;
}

bool Input::IsKeyRelease(KeyCode key) const
{
	return m_keyStates.at(static_cast<std::underlying_type_t<KeyCode>>(key)) == KeyAction::RELEASE;
}

bool Input::IsKeyDown(KeyCode key) const
{
	return m_keyStates.at(static_cast<std::underlying_type_t<KeyCode>>(key)) == KeyAction::PRESS ||
		m_keyStates.at(static_cast<std::underlying_type_t<KeyCode>>(key)) == KeyAction::REPEAT;
}

bool Input::IsMouseButtonPress(MouseButton button) const
{
	return m_mouseButtonStates.at(static_cast<std::underlying_type_t<MouseButton>>(button)) == KeyAction::PRESS;
}

bool Input::IsMouseButtonRelease(MouseButton button) const
{
	return m_mouseButtonStates.at(static_cast<std::underlying_type_t<MouseButton>>(button)) == KeyAction::RELEASE;

}

bool Input::IsMouseButtonDown(MouseButton button) const
{
	return m_mouseButtonStates.at(static_cast<std::underlying_type_t<MouseButton>>(button)) == KeyAction::PRESS ||
		m_mouseButtonStates.at(static_cast<std::underlying_type_t<MouseButton>>(button)) == KeyAction::REPEAT;
}

double Input::GetMouseX() const
{
	return m_mousePosX;
}

double Input::GetMouseY() const
{
	return m_mousePosY;
}

double Input::GetDeltaMouseX() const
{
	return m_mousePosX - m_prevMousePosX;
}

double Input::GetDeltaMouseY() const
{
	return m_mousePosY - m_prevMousePosY;
}

void Input::Update()
{
	for (int i = 0; i < m_keyStates.size(); ++i)
	{
		if (m_previousKeyStates[i] == KeyAction::PRESS)
			m_keyStates[i] = KeyAction::REPEAT;
		else if (m_previousKeyStates[i] == KeyAction::RELEASE)
			m_keyStates[i] = KeyAction::NONE;

		m_previousKeyStates[i] = m_keyStates[i];
	}

	for (int i = 0; i < m_mouseButtonStates.size(); ++i)
	{
		if (m_previousMouseButtonStates[i] == KeyAction::PRESS)
			m_mouseButtonStates[i] = KeyAction::REPEAT;
		else if (m_previousMouseButtonStates[i] == KeyAction::RELEASE)
			m_mouseButtonStates[i] = KeyAction::NONE;

		m_previousMouseButtonStates[i] = m_mouseButtonStates[i];
	}

	m_prevMousePosX = m_mousePosX;
	m_prevMousePosY = m_mousePosY;
	m_mousePosX = m_callbackMousePosX;
	m_mousePosY = m_callbackMousePosY;
}

void Input::RegisterMouseCursorButton(double xPos, double yPos)
{
	m_callbackMousePosX = xPos;
	m_callbackMousePosY = yPos;
}
