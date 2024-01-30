#include "input/input.h"
#include "core/Log.h"
#include <format>

using namespace INF;

void Input::RegisterKey(KeyCode key, int scancode, KeyAction action, int mods)
{
	Log::Info(std::format("Key = {}, Action = {}", (int)key, (int)action));
}

