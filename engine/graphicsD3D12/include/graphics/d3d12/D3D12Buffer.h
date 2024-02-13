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
	private:
		D3D12Device* m_device;
		BufferDesc m_desc;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
	};
}