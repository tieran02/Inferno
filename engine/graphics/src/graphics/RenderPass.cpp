#include "infPCH.h"
#include "graphics/RenderPass.h"
#include "graphics/interface/comandlist.h"
#include "graphics/interface/Texture.h"


using namespace INF::GFX;

void IGeometryPass::Render(ICommandList* commandList, IFramebuffer* framebuffer)
{
	GraphicsState state;
	SetState(state);
	state.framebuffer = framebuffer;

	MeshInstance** meshInstances;
	uint32_t meshCount = 0;
	GetMeshInstances(meshInstances, meshCount);

	for (int i = 0; i < meshCount; ++i)
	{
		const MeshInstance* instance = meshInstances[i];
		const MeshInfo* meshInfo = instance->mesh;

		state.vertexBuffer = meshInfo->buffer.vertexBuffer.get();
		state.indexBuffer = meshInfo->buffer.indexBuffer.get();
		OnMeshInstanceRender(i, state);

		commandList->SetGraphicsState(state);

		commandList->DrawIndexed(meshInfo->numIndices, 1, meshInfo->indexOffset, meshInfo->vertexOffset, instance->instanceOffset);
	}
}
