#pragma once
#include <memory>
#include "graphics/graphicDefines.h"

namespace INF::GFX
{
	class IBuffer;
	class ITexture;
	struct GraphicsState;
	struct Color;
	class Bitmap;

	class ICommandList
	{
	public:
		virtual ~ICommandList() = default;
		virtual void Open() = 0;
		virtual void Close() = 0;

		virtual void ClearColor(ITexture* texture, const Color& color) = 0;
		virtual void ClearDepth(ITexture* texture, float depth, uint8_t stencil) = 0;

		virtual void SetGraphicsState(const GraphicsState& state) = 0;
		virtual void SetViewport(const Viewport& viewport) = 0;
		virtual void SetScissor(const Rect& scissor) = 0;

		virtual void WriteBuffer(IBuffer* dest, const void* src, size_t size, size_t destOffset = 0) = 0;
		virtual void CopyBuffer(IBuffer* dest, uint32_t destOffset, IBuffer* src, uint32_t srcOffset, size_t size) = 0;
		virtual void WriteTexture(ITexture* dest, const Bitmap& bitmap) = 0;

		virtual void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
		virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, uint32_t firstVertex, uint32_t firstInstance) = 0;
	};

	using CommandListeHandle = std::unique_ptr<ICommandList>;

}