#pragma once
#include "graphicDefines.h"
#include "interface/device.h"
#include "core/Transform.h"

namespace INF::GFX
{
	struct MeshBuffer
	{
		BufferHandle indexBuffer;
		BufferHandle vertexBuffer;
	};

	struct MeshInfo
	{
		std::string name;
		MeshBuffer buffer;
		uint32_t indexOffset;
		uint32_t vertexOffset;
		uint32_t numIndices;
		uint32_t numVertices;
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
		virtual std::string_view GetName() = 0;
	};

	class IGeometryPass : public IRenderPass
	{
		virtual void Prepare(ICommandList* commandList, const View& view) = 0;
		virtual void SetPipeline(GraphicsState& state) = 0;
		void Render(ICommandList* commandList, const View& view, IFramebuffer* framebuffer, std::vector<MeshInstance*>& meshInstances);
	};

	class IFullscreenPass : public IRenderPass
	{

	};
}