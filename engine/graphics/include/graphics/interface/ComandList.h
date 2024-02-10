#pragma once
#include <memory>
#include "graphics/graphicDefines.h"

namespace INF::GFX
{
	class ITexture;
	struct Color;

	class ICommandList
	{
	public:
		virtual ~ICommandList() = default;
		virtual void Open() = 0;
		virtual void Close() = 0;

		virtual void Transition(ITexture* texture, TRANSITION_STATES_FLAGS from, TRANSITION_STATES_FLAGS to) = 0;
		virtual void ClearColor(ITexture* texture, const Color& color) = 0;
	};

	using CommandListeHandle = std::unique_ptr<ICommandList>;

}