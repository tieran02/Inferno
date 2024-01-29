#include "core/log.h"

#include "spdlog/spdlog.h"

using namespace Inferno;

void Log::Trace(std::string_view msg)
{
	spdlog::trace(msg);
}

void Log::Info(std::string_view msg)
{
	spdlog::info(msg);
}

void Log::Warning(std::string_view msg)
{
	spdlog::warn(msg);
}

void Log::Error(std::string_view msg)
{
	spdlog::error(msg);
}

void Log::Critical(std::string_view msg)
{
	spdlog::critical(msg);
	__debugbreak();
}

