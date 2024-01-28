#pragma once
#include <string_view>

namespace Inferno
{
	class Log
	{
	public:
		static void Info(std::string_view msg);
	};
}