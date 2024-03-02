#pragma once
#include "graphics/interface/ComandList.h"
#include "graphics/graphicDefines.h"
#include "directx/d3d12.h"
#include "wrl/client.h"
#include "graphics/interface/Buffer.h"

namespace INF::GFX
{
	class D3D12GraphicsPipeline;
	class D3D12Framebuffer;
	class D3D12DescriptorSet;
	class D3D12Device;

	class D3D12Queue
	{
	public:
		D3D12Queue(ID3D12Device* d3dDevice, CommandQueue queueType);
		~D3D12Queue();
		uint64_t ExecuteCommandLists(ID3D12GraphicsCommandList* commandLists, uint32_t commandListCount);
		void Wait();
		ID3D12CommandQueue* D3D() const { return Queue.Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> Queue;
		Microsoft::WRL::ComPtr<ID3D12Fence> Fence;
		HANDLE m_fenceEvent;
		uint64_t LastSubmittedInstance = 0;
		uint64_t LastCompletedInstance = 0;
		uint64_t UpdateLastCompletedInstance();
	};
	using D3D12QueueHandle = std::shared_ptr<D3D12Queue>;

	class D3D12CommandList : public ICommandList
	{
	public:
		D3D12CommandList(D3D12Device* d3dDevice, ID3D12CommandAllocator* commandAllocator, CommandQueue queueType);

		void Open() override;
		void Close() override;

		void ClearColor(ITexture* texture, const Color& color) override;

		void SetGraphicsState(const GraphicsState& state) override;

		void SetViewport(const Viewport& viewport) override;
		void SetScissor(const Rect& scissor) override;

		void CopyBuffer(IBuffer* dest, uint32_t destOffset, IBuffer* src, uint32_t srcOffset, size_t size) override;
		void WriteTexture(ITexture* dest, const Bitmap& bitmap) override;


		void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;

		ID3D12GraphicsCommandList* D3D() const { return m_commandList.Get(); }

	private:
		void TransitionResource(ID3D12Resource* resource, TRANSITION_STATES_FLAGS from, TRANSITION_STATES_FLAGS to);
		TRANSITION_STATES_FLAGS GetTextureState(ITexture* texture);
		void Transition(ITexture* texture, TRANSITION_STATES_FLAGS to);
		TRANSITION_STATES_FLAGS GetBufferState(IBuffer* buffer);
		void Transition(IBuffer* buffer, TRANSITION_STATES_FLAGS to);

		void BindGraphicsPipeline(D3D12GraphicsPipeline* pso);
		void BindFramebuffer(D3D12GraphicsPipeline* pso, D3D12Framebuffer* fb);
		void BindDescriptorSet(D3D12DescriptorSet* set);

		ID3D12CommandAllocator* m_commandAllocator; //Ref to the command allocator
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		std::vector<BufferHandle> m_referencedBuffers;

		std::unordered_map<ITexture*, TRANSITION_STATES_FLAGS> m_textureStates;
		std::unordered_map<IBuffer*, TRANSITION_STATES_FLAGS> m_bufferStates;

		D3D12Device* m_device;
	};
}