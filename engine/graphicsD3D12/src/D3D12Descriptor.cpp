#include "infPCH.h"
#include "graphics/d3d12/D3D12Descriptor.h"
#include "directx/d3dx12_root_signature.h"
#include "core/Assert.h"
#include "graphics/d3d12/D3D12Defines.h"
#include <format>
#include "graphics/d3d12/D3D12Device.h"

using namespace INF::GFX;

D3D12DescriptorLayout::D3D12DescriptorLayout(ID3D12Device* device, const DescriptorLayoutDesc& desc) : m_desc(desc)
{
	SetStageRanges(ShaderType::Vertex, desc.VS);
	SetStageRanges(ShaderType::Pixel, desc.PS);
	SetStageRanges(ShaderType::All, desc.ALL);

	//Create root parameters from the ranges collected above
	for (int i = 0; i < m_srvRanges.size(); ++i)
	{
		D3D12_ROOT_PARAMETER1 param;

		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param.ShaderVisibility = D3D12ShaderVisibility(m_srvRanges[i].second);
		param.DescriptorTable.NumDescriptorRanges = 1;
		param.DescriptorTable.pDescriptorRanges = &m_srvRanges[i].first;

		m_rootParameters.push_back(param);
	}

	for (int i = 0; i < m_samplerRanges.size(); ++i)
	{
		D3D12_ROOT_PARAMETER1 param;

		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param.ShaderVisibility = D3D12ShaderVisibility(m_samplerRanges[i].second);
		param.DescriptorTable.NumDescriptorRanges = 1;
		param.DescriptorTable.pDescriptorRanges = &m_samplerRanges[i].first;

		m_rootParameters.push_back(param);
	}
}


void D3D12DescriptorLayout::SetStageRanges(ShaderType shaderType, const StageBindingDescriptorDesc& desc)
{
	int descriptorTableSizeSamplers = 0;
	int descriptorTableSizeSRVetc = 0;

	for (const DescriptorLayoutItem& layoutItem : desc)
	{
		if (layoutItem.slot == UINT_MAX)
			continue;

		if (layoutItem.type == ResourceType::SAMPLER)
		{
			D3D12_DESCRIPTOR_RANGE1 range{};

			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			range.NumDescriptors = 1;
			range.BaseShaderRegister = layoutItem.slot;
			range.RegisterSpace = layoutItem.registerSpace;
			range.OffsetInDescriptorsFromTableStart = descriptorTableSizeSRVetc;
			range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

			m_samplerRanges.push_back(std::make_pair(range, shaderType));
		}
		else
		{
			D3D12_DESCRIPTOR_RANGE1 range{};

			switch (layoutItem.type)
			{
			case ResourceType::TEXTURE_SRV:
			case ResourceType::BUFFER_SRV:
				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				break;
			case ResourceType::CONSTANTBUFFER:
				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
				break;
			default:
				INF_ASSERT(false, "ResourceType not supported");
			}
			range.NumDescriptors = 1;
			range.BaseShaderRegister = layoutItem.slot;
			range.RegisterSpace = layoutItem.registerSpace;
			range.OffsetInDescriptorsFromTableStart = descriptorTableSizeSamplers;

			m_srvRanges.push_back(std::make_pair(range,shaderType));
		}
	}
}

const INF::GFX::DescriptorLayoutDesc& D3D12DescriptorLayout::GetDesc() const
{
	return m_desc;
}

D3D12DescriptorSet::D3D12DescriptorSet(IDescriptorLayout* layout, const DescriptorSetDesc& setDesc) : m_desc(setDesc), m_layout(layout)
{
	INF_ASSERT(layout, "Layout can't be null");
}

const DescriptorSetDesc& D3D12DescriptorSet::GetDesc() const
{
	return m_desc;
}

IDescriptorLayout* D3D12DescriptorSet::GetLayout() const
{
	return m_layout;
}


D3D12DescriptorLayoutCache::D3D12DescriptorLayoutCache(D3D12Device* device) : m_device(device)
{

}

ID3D12RootSignature* D3D12DescriptorLayoutCache::GetRootSignature(const DescriptorLayoutGroup& descriptorLayoutSet)
{
	size_t hash = std::hash<DescriptorLayoutGroup>()(descriptorLayoutSet);
	auto it = m_setRootSignatureMap.find(hash);

	if (it != m_setRootSignatureMap.end())
		return it->second.Get();

	//Create a new root signature from the descriptor sets

	std::vector<D3D12_ROOT_PARAMETER1> combinedRootParams;

	for (const DescriptorLayoutHandle& layout : descriptorLayoutSet)
	{
		if(!layout)
			continue;

		const D3D12DescriptorLayout* d3d12Layout = static_cast<const D3D12DescriptorLayout*>(layout.get());
		const auto& rootParamters = d3d12Layout->RootParamters();
		combinedRootParams.insert(combinedRootParams.end(), rootParamters.begin(), rootParamters.end());
	}

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
	rsDesc.Desc_1_1.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	if (!combinedRootParams.empty())
	{
		rsDesc.Desc_1_1.pParameters = combinedRootParams.data();
		rsDesc.Desc_1_1.NumParameters = UINT(combinedRootParams.size());
	}

	Microsoft::WRL::ComPtr<ID3DBlob> rsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	VerifySuccess(D3D12SerializeVersionedRootSignature(&rsDesc, &rsBlob, &errorBlob));
	if (errorBlob)
	{
		std::string error = std::string((char*)errorBlob->GetBufferPointer());
		Log::Error(error);
	}

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	VerifySuccess(m_device->Device()->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));

	it = m_setRootSignatureMap.emplace(hash, rootSignature).first;
	return it->second.Get();
}

uint32_t D3D12DescriptorLayoutCache::GetRootParamterIndex(const DescriptorLayoutGroup& set, ShaderType stage, const DescriptorLayoutItem& layoutItem)
{
	size_t hash = HashRootParamterIndex(set, stage ,layoutItem);
	auto it = m_rootParamterIndexMap.find(hash);

	INF_ASSERT(it != m_rootParamterIndexMap.end(), "Root paramter doesn't exist within this DescriptorLayoutSet");

	if (it != m_rootParamterIndexMap.end())
		return it->second;

	return 0;
}

size_t D3D12DescriptorLayoutCache::HashRootParamterIndex(const DescriptorLayoutGroup& set, ShaderType stage, const DescriptorLayoutItem& layoutItem)
{
	size_t hash = std::hash<DescriptorLayoutGroup>()(set);
	hash ^= std::hash<uint8_t>()((uint8_t)stage);
	hash ^= std::hash<DescriptorLayoutItem>()(layoutItem);
	return hash;
}
