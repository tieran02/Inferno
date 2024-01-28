// main.cpp
#include <iostream>
#include "core/Log.h"
#include <format>

using namespace std;

int main(int argc, char** argv)
{
	Inferno::Log::Info(std::format("Hello, {0}!", "Sharpmake"));
}