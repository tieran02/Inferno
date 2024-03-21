#pragma once
#include "graphicDefines.h"
#include "interface/device.h"
#include "core/Transform.h"
#include "graphics/MeshGenerator.h"

namespace INF::GFX
{
	struct MeshInstance;

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

		virtual void SetState(GraphicsState& state) = 0;
		virtual void GetMeshInstances(MeshInstance**& instances, uint32_t& meshCount) = 0;

		virtual void OnMeshInstanceRender(uint32_t meshInstanceIndex, GraphicsState& state) {}

		virtual void Render(ICommandList* commandList, IFramebuffer* framebuffer);
	};
}