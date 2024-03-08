#pragma once
#include "graphics/interface/Buffer.h"
#include <wrl/client.h>
#include "directx/d3d12.h"
#include "graphics/d3d12/D3D12Device.h"


namespace INF::GFX
{
	class D3D12Device;

	class D3D12BufferView : public IBufferView
	{
	public:
		DescriptorIndex descriptorIndex{ DescriptorIndexInvalid };
		D3D12_CPU_DESCRIPTOR_HANDLE CPU{ 0 };
		D3D12_GPU_DESCRIPTOR_HANDLE GPU{ 0 };
	};

	class D3D12Buffer : public IBuffer
	{
	public:
		D3D12Buffer(D3D12Device* device, const BufferDesc& desc);
		~D3D12Buffer();
		BufferDesc GetDesc() override;
		IBufferView* GetView() override;

		ID3D12Resource* Resource() { return m_resource.Get(); }

		void CreateConstantBufferView(DescriptorIndex descriptorIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress);
	private:
		D3D12Device* m_device;
		BufferDesc m_desc;
		D3D12BufferView m_view;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
	};


	class D3D12VertexBuffer : public IVertexBuffer
	{
	public:
		D3D12VertexBuffer(D3D12Device* device, VertexBufferDesc desc);
		IBuffer* GetBuffer() override;
		VertexBufferDesc GetDesc() override;
		const D3D12_VERTEX_BUFFER_VIEW& View() const { return m_view; }
	private:
		D3D12Buffer m_buffer;
		VertexBufferDesc m_desc;
		D3D12_VERTEX_BUFFER_VIEW m_view;
	};

	class D3D12IndexBuffer: public IIndexBuffer
	{
	public:
		D3D12IndexBuffer(D3D12Device* device, IndexBufferDesc desc);
		IBuffer* GetBuffer() override;
		IndexBufferDesc GetDesc() override;
		const D3D12_INDEX_BUFFER_VIEW& View() const { return m_view; }
	private:
		D3D12Buffer m_buffer;
		IndexBufferDesc m_desc;
		D3D12_INDEX_BUFFER_VIEW m_view;
	};


	//D3D12 requires a buffer for each frame for data that changes between frames, for example a constant buffer
	//We can get the developer to create a buffer for each frame but that requires more management outside the engine and passing around the current frame index
	//Instead create a heap of memory that is CPU read/write (upload heap) that we can use to allocate the buffer data and have multiple versions per frame
	//Unfortunately this means we can't persistently map d3d12 constant buffers but we do get the benefit of having constant data in one large buffer
	class FrameBufferMemory
	{
	public:
		static const uint32_t ALIGNMENT = 4096;
		static std::shared_ptr<FrameBufferMemory> Create(D3D12Device* device, size_t size);
		~FrameBufferMemory();

		void Reset();
		bool Allocate(size_t size, ID3D12Resource** pBuffer, size_t* pOffset, void** pCpuVA, D3D12_GPU_VIRTUAL_ADDRESS* pGpuVA, uint32_t alignment, DescriptorIndex& descriptorIndex);
		void SetName(const wchar_t* name);
	private:
		FrameBufferMemory(D3D12Device* device, size_t size);

		D3D12Device* m_device;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer;
		size_t m_bufferSize = 0;
		size_t m_freeDataLocation = 0;
		void* m_cpuVirtualAddres = nullptr;
		D3D12_GPU_VIRTUAL_ADDRESS m_gpuVirtualAddress = 0;
		std::vector<DescriptorIndex> m_descriptorIndices;
	};

	class FrameBufferManager
	{

	};
}