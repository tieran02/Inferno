#pragma once
#include "core/Assert.h"
#include "graphics/GraphicDefines.h"
#include "graphics/interface/comandlist.h"
#include "graphics/interface/Shader.h"
#include "graphics/interface/Texture.h"
#include "graphics/interface/Pipeline.h"

namespace INF::GFX
{
	class IDevice;
	using DeviceHandle = std::unique_ptr<IDevice>;

	class IDevice
	{
	public:
		virtual ~IDevice() = default;
		virtual ShaderHandle CreateShader(const ShaderDesc& desc) = 0;

		virtual FramebufferHandle CreateFramebuffer(const FramebufferDesc& desc) = 0;
		virtual GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb) = 0;

		virtual CommandListeHandle CreateCommandList(CommandQueue queueType) = 0;
		virtual uint64_t ExecuteCommandLists(const ICommandList* commandLists, uint32_t commandListCount) = 0;

		virtual void WaitForIdle() = 0;
	};
}