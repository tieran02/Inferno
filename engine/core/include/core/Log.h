#pragma once

namespace INF
{
	class Log
	{
	public:
		static void Trace(std::string_view msg);
		static void Info(std::string_view msg);
		static void Warning(std::string_view msg);
		static void Error(std::string_view msg);
		static void Critical(std::string_view msg);
	};
}