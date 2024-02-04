#pragma once
#include <memory>

namespace INF::GFX
{
	class ICommandList
	{
	public:
		virtual void Open() = 0;
		virtual void Close() = 0;
	};

	using CommandListeHandle = std::unique_ptr<ICommandList>;

}