#pragma once
#include <memory>
#include "graphics/GraphicDefines.h"

namespace INF::GFX
{
	enum class CpuVisible
	{
		NONE,
		READ,
		WRITE
	};

	enum class BufferUsage
	{
		GENERIC,
		VERTEX,
		INDEX,
		CONSTANT,
	};

	struct BufferDesc
	{
		uint32_t byteSize = 0;
		const char* name = nullptr;
		TRANSITION_STATES_FLAGS initialState = (TRANSITION_STATES_FLAGS)TRANSITION_STATES::COMMON;
		BufferUsage usage = BufferUsage::GENERIC;
		CpuVisible access = CpuVisible::NONE;
	};

	class IBuffer
	{
	public:
		virtual BufferDesc GetDesc() = 0;
	};

	using BufferHandle = std::shared_ptr<IBuffer>;
}