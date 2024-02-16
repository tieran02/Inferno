#pragma once
#include "core/Assert.h"
#include "graphics/GraphicDefines.h"
#include "graphics/interface/comandlist.h"
#include "graphics/interface/Shader.h"
#include "graphics/interface/Texture.h"
#include "graphics/interface/Pipeline.h"
#include "graphics/interface/Buffer.h"
#include "graphics/interface/Descriptor.h"
#include <functional>


namespace INF::GFX
{
	class IDevice;
	using DeviceHandle = std::unique_ptr<IDevice>;

	using ImmediateSubmitFn = std::function<void(ICommandList* cmd)>;

	class IDevice
	{
	public:
		virtual ~IDevice() = default;
		virtual ShaderHandle CreateShader(const ShaderDesc& desc) = 0;

		virtual FramebufferHandle CreateFramebuffer(const FramebufferDesc& desc) = 0;
		virtual GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb) = 0;
		virtual DescriptorLayoutHandle CreateDescriptorLayout(const DescriptorLayoutDesc desc) = 0;
		virtual DescriptorSetHandle CreateDescriptorSet(const DescriptorSetDesc& desc, IDescriptorLayout* layout) = 0;

		virtual BufferHandle CreateBuffer(const BufferDesc& desc) = 0;
		virtual VertexBufferHandle CreateVertexBuffer(const VertexBufferDesc& desc) = 0;
		virtual IndexBufferHandle CreateIndexBuffer(const IndexBufferDesc& desc) = 0;
		virtual void* MapBuffer(IBuffer* buffer, uint32_t readStart = 0, uint32_t readEnd = 0) = 0;
		virtual void UnmapBuffer(IBuffer* buffer, uint32_t writeStart = 0, uint32_t writeEnd = 0) = 0;

		virtual SamplerHandle CreateSampler(const SamplerDesc& desc) = 0;
		virtual TextureHandle CreateTexture(const TextureDesc& desc) = 0;

		virtual CommandListeHandle CreateCommandList(CommandQueue queueType) = 0;
		virtual uint64_t ExecuteCommandLists(const ICommandList* commandLists, uint32_t commandListCount) = 0;
		virtual void ImmediateSubmit(ImmediateSubmitFn ImmediateFn) = 0;

		virtual void WaitForIdle() = 0;
	};
}