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
		std::string name;
		TRANSITION_STATES_FLAGS initialState = (TRANSITION_STATES_FLAGS)TRANSITION_STATES::COMMON;
		BufferUsage usage = BufferUsage::GENERIC;
		CpuVisible access = CpuVisible::NONE;
	};

	class IBufferView
	{
		Format format;
	};

	class IBuffer
	{
	public:
		virtual ~IBuffer() = default;

		virtual BufferDesc GetDesc() = 0;
		virtual IBufferView* GetView() = 0;
	};

	using BufferHandle = std::shared_ptr<IBuffer>;


	struct VertexBufferDesc
	{
		uint32_t byteSize = 0;
		uint32_t strideInBytes = 0;
		std::string name;
		CpuVisible access = CpuVisible::NONE;
	};

	class IVertexBuffer
	{
	public:
		virtual IBuffer* GetBuffer() = 0;
		virtual VertexBufferDesc GetDesc() = 0;
	};
	using VertexBufferHandle = std::shared_ptr<IVertexBuffer>;

	struct IndexBufferDesc
	{
		uint32_t byteSize = 0;
		Format format = Format::R16_UINT;
		std::string name;
		CpuVisible access = CpuVisible::NONE;
	};

	class IIndexBuffer
	{
	public:
		virtual IBuffer* GetBuffer() = 0;
		virtual IndexBufferDesc GetDesc() = 0;
	};
	using IndexBufferHandle = std::shared_ptr<IIndexBuffer>;
}