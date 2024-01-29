// main.cpp
#include <iostream>
#include "core/Log.h"
#include <format>
#include "core/Assert.h"

using namespace Inferno;

int main(int argc, char** argv)
{
	Log::Trace(std::format("Hello, {0}!", "Sharpmake"));
	Log::Info(std::format("Hello, {0}!", "Sharpmake"));
	Log::Warning(std::format("Hello, {0}!", "Sharpmake"));

	INF_ASSERT(false, "Test Assert");

	Log::Error(std::format("Hello, {0}!", "Sharpmake"));
}