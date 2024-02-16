#pragma once
#include "graphics/interface/device.h"
#include "graphics/interface/deviceManager.h"
#include "directx/d3dx12.h"
#include <wrl/client.h>
#include <dxgi1_4.h>
#include "graphics/d3d12/D3D12CommandList.h"

namespace INF::GFX
{
	class ITexture;
	using DescriptorIndex = uint32_t;
	constexpr DescriptorIndex DescriptorIndexInvalid = UINT_MAX;

	class D3D12DescriptorHeap
	{
	public:
		bool CreateResources(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t descriptorCount, bool shaderVisible);
		DescriptorIndex AllocateDescriptor();
		void ReleaseDescriptor(DescriptorIndex);
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(DescriptorIndex index);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(DescriptorIndex index);
		ID3D12DescriptorHeap* Heap() const { return m_heap.Get(); }
	private:
		struct D3D12Descritpor
		{
			D3D12_CPU_DESCRIPTOR_HANDLE Cpu;
			D3D12_GPU_DESCRIPTOR_HANDLE Gpu;
		};

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
		D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;

		std::vector<D3D12Descritpor> m_descriptors;
		std::deque<uint32_t> m_freeDescriptors; //Indices of free descriptors in m_descriptors
	};

	class D3D12Device : public IDevice
	{
	public:
		D3D12Device(const DeviceCreationParameters& createInfo);

		ShaderHandle CreateShader(const ShaderDesc& desc) override;

		FramebufferHandle CreateFramebuffer(const FramebufferDesc& desc) override;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> CreatePipelineState(const GraphicsPipelineDesc& desc, IFramebuffer* fb, ID3D12RootSignature* rootSignature);
		GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb) override;
		DescriptorLayoutHandle CreateDescriptorLayout(const DescriptorLayoutDesc desc) override;
		DescriptorSetHandle CreateDescriptorSet(const DescriptorSetDesc& desc, IDescriptorLayout* layout) override;

		BufferHandle CreateBuffer(const BufferDesc& desc) override;
		VertexBufferHandle CreateVertexBuffer(const VertexBufferDesc& desc) override;
		IndexBufferHandle CreateIndexBuffer(const IndexBufferDesc& desc) override;
		void* MapBuffer(IBuffer* buffer, uint32_t readStart = 0, uint32_t readEnd = 0) override;
		void UnmapBuffer(IBuffer* buffer, uint32_t writeStart = 0, uint32_t writeEnd = 0) override;

		SamplerHandle CreateSampler(const SamplerDesc& desc) override;
		TextureHandle CreateTexture(const TextureDesc& desc) override;

		CommandListeHandle CreateCommandList(CommandQueue queueType) override;
		uint64_t ExecuteCommandLists(const ICommandList* commandLists, uint32_t commandListCount) override;
		void ImmediateSubmit(ImmediateSubmitFn ImmediateFn) override;

		void WaitForIdle() override;

		void CreateRenderTargetView(DescriptorIndex descriptorIndex, ITexture* texture);
		void CreateShaderResourceView(DescriptorIndex descriptorIndex, ITexture* texture);

		IDXGIFactory4* Factory() { return m_dxgiFactory.Get(); }
		IDXGIAdapter1* Adapter() { return m_adapter.Get(); }
		ID3D12Device*  Device() { return  m_device.Get(); }
		D3D12QueueHandle GetGraphicsQueue() { return m_graphicsQueue; }

		D3D12DescriptorHeap& SRVDescriptoHeap() { return m_SRVDescriptorHeap; }
		D3D12DescriptorHeap& RTVDescriptoHeap() { return m_RTVDescriptorHeap; }
		D3D12DescriptorHeap& DSVDescriptoHeap() { return m_DSVDescriptorHeap; }
		D3D12DescriptorHeap& SamplerDescriptoHeap() { return m_SamplerDescriptorHeap; }
	private:
		void CreateDebugController();
		void CreateFactory();
		void CreateAdapter();
		void CreateGraphicsCommandAllocator();
		void CreateDescriptorHeaps();

		//Device members
		Microsoft::WRL::ComPtr<ID3D12Debug1> m_debugController;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> m_adapter;
		Microsoft::WRL::ComPtr<ID3D12Device> m_device;	
		Microsoft::WRL::ComPtr<ID3D12DebugDevice> m_debugDevice;

		//Context members
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_graphicsCommandAllocator;
		D3D12QueueHandle m_graphicsQueue;

		D3D12DescriptorHeap m_SRVDescriptorHeap;
		D3D12DescriptorHeap m_RTVDescriptorHeap;
		D3D12DescriptorHeap m_DSVDescriptorHeap;
		D3D12DescriptorHeap m_SamplerDescriptorHeap;
	};
}