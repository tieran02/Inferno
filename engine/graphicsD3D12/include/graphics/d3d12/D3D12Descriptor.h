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

	class D3D12DescriptorSet : public IDescriptorSet
	{
	public:
		D3D12DescriptorSet(ID3D12Device* device, IDescriptorLayout* layout, const DescriptorSetDesc& setDesc);

		const DescriptorSetDesc& GetDesc() const override;
		IDescriptorLayout* GetLayout() const override;

		uint32_t GetRootParamIndex(const DescriptorSetItem& setItem);
	private:
		DescriptorSetDesc m_desc;
		IDescriptorLayout* m_layout;

		std::unordered_map<DescriptorLayoutItem, uint32_t> m_layoutRootIndexMap;
		//std::unordered_map<const DescriptorSetItem*, uint32_t> m_rootParamterIndexMap; //Key is set item that contains a ptr to the resource, value the root parameter index (slot)
	};
}