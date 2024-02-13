#pragma once
#include "graphics/interface/Buffer.h"
#include <wrl/client.h>
#include "directx/d3d12.h"


namespace INF::GFX
{
	class D3D12Device;
	class D3D12Buffer : public IBuffer
	{
	public:
		D3D12Buffer(D3D12Device* device, const BufferDesc& desc);
		BufferDesc GetDesc() override;
		ID3D12Resource* Resource() { return m_resource.Get(); }
	private:
		D3D12Device* m_device;
		BufferDesc m_desc;
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
}