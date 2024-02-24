#include "infPCH.h"
#include "graphics/RenderPass.h"
#include "graphics/interface/comandlist.h"
#include "graphics/interface/Texture.h"

using namespace INF::GFX;

void IGeometryPass::Render(ICommandList* commandList, const View& view, IFramebuffer* framebuffer, std::vector<MeshInstance*>& meshInstances)
{
	Prepare(commandList, view);

	GraphicsState graphicsState;
	graphicsState.framebuffer = framebuffer;
	graphicsState.view = &view;

	//Set descriptors and pipeline, this would be set material when implemented
	SetPipeline(graphicsState);

	for (MeshInstance* meshInstance : meshInstances)
	{
		//graphicsState.vertexBuffer = meshInstance->mesh->buffer.vertexBuffer.get();
	}
}
