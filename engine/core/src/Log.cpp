#include "core/log.h"

#include "spdlog/spdlog.h"

using namespace Inferno;

void Log::Info(std::string_view msg)
{
	spdlog::info(msg);
}

