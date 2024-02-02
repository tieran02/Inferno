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
	m_keyStates[static_cast<std::underlying_type_t<KeyCode>>(key)] = action;
}

bool Input::IsKeyPress(KeyCode key)
{
	return m_keyStates[static_cast<std::underlying_type_t<KeyCode>>(key)] == KeyAction::PRESS;
}

bool Input::IsKeyRelease(KeyCode key)
{
	return m_keyStates[static_cast<std::underlying_type_t<KeyCode>>(key)] == KeyAction::RELEASE;
}

bool Input::IsKeyDown(KeyCode key)
{
	return m_keyStates[static_cast<std::underlying_type_t<KeyCode>>(key)] == KeyAction::PRESS ||
		m_keyStates[static_cast<std::underlying_type_t<KeyCode>>(key)] == KeyAction::REPEAT;
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
}

