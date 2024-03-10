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

		const std::vector<D3D12_ROOT_PARAMETER1>& RootParamters() const { return m_rootParameters; }
	private:
		void SetStageRanges(ShaderType type, const StageBindingDescriptorDesc& desc);

		DescriptorLayoutDesc m_desc;

		std::vector<std::pair<D3D12_DESCRIPTOR_RANGE1, ShaderType>> m_srvRanges;
		std::vector<std::pair<D3D12_DESCRIPTOR_RANGE1, ShaderType>> m_samplerRanges;
		std::vector<D3D12_ROOT_PARAMETER1> m_rootParameters;

	};

	class D3D12DescriptorSet : public IDescriptorSet
	{
	public:
		D3D12DescriptorSet(IDescriptorLayout* layout, const DescriptorSetDesc& setDesc);

		const DescriptorSetDesc& GetDesc() const override;
		IDescriptorLayout* GetLayout() const override;
	private:
		DescriptorSetDesc m_desc;
		IDescriptorLayout* m_layout;
	};

	class D3D12Device;
	class D3D12DescriptorLayoutCache
	{
	public:
		D3D12DescriptorLayoutCache(D3D12Device* device);

		ID3D12RootSignature* GetRootSignature(const DescriptorLayoutGroup& set);
		uint32_t GetRootParamterIndex(const DescriptorLayoutGroup& set, ShaderType stage, const DescriptorLayoutItem& layoutItem);
	private:
		D3D12Device* m_device;
		size_t HashRootParamterIndex(const DescriptorLayoutGroup& set, ShaderType stage, const DescriptorLayoutItem& layoutItem);

		//Size_t is the hash of DescriptorLayoutSet std::hash<DescriptorLayoutSet>
		std::unordered_map<size_t, Microsoft::WRL::ComPtr<ID3D12RootSignature>> m_setRootSignatureMap;

		std::unordered_map<size_t, uint32_t> m_rootParamterIndexMap;
	};
}