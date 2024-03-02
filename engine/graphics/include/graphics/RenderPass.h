#pragma once
#include "graphicDefines.h"
#include "interface/device.h"
#include "core/Transform.h"

namespace INF::GFX
{
	struct MeshBuffer
	{
		IndexBufferHandle indexBuffer;
		VertexBufferHandle vertexBuffer;
	};

	struct MeshInfo
	{
		std::string name;
		MeshBuffer buffer;
		uint32_t indexOffset{ 0 };
		uint32_t vertexOffset{ 0 };
		uint32_t numIndices{ 0 };
		uint32_t numVertices{ 0 };
	};

	struct MeshInstance
	{
		MeshInfo* mesh;
		Transform transform;
		uint32_t instanceOffset;
	};

	class IRenderPass
	{
	public:
		virtual void Render(ICommandList* commandList, IFramebuffer* framebuffer) = 0;

		virtual std::string_view GetName() = 0;
	};

	class IGeometryPass : public IRenderPass
	{
	public:
		virtual void Prepare(ICommandList* commandList, const View& view, MeshInstance** meshInstances, uint32_t meshCount) = 0;
	};
}