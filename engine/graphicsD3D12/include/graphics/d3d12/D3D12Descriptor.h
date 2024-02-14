#pragma once
#include "graphics/interface/Descriptor.h"
#include "directx/d3d12.h"
#include <wrl/client.h>

namespace INF::GFX
{
	class D3D12DescriptorLayout : public IDescriptorLayout
	{
	public:
		D3D12DescriptorLayout(ID3D12Device* device, const DescriptorLayoutDesc& desc);

		const DescriptorLayoutDesc& GetDesc() const override;
		ID3D12RootSignature* RootSignature() const { return m_rootSignature.Get(); }
	private:
		DescriptorLayoutDesc m_desc;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
	};
}