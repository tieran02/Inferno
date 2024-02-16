#include "infPCH.h"
#include "graphics/d3d12/D3D12Descriptor.h"
#include "directx/d3dx12_root_signature.h"
#include "core/Assert.h"
#include "graphics/d3d12/D3D12Defines.h"
#include <format>

using namespace INF::GFX;

void GetRootRanges(StageBindingDescriptorDesc stage, std::vector<D3D12_DESCRIPTOR_RANGE1>& outSrvRanges, std::vector<D3D12_DESCRIPTOR_RANGE1>& outSamplerRanges)
{
	outSrvRanges.clear();
	outSamplerRanges.clear();

	int descriptorTableSizeSamplers = 0;
	int descriptorTableSizeSRVetc = 0;

	for (const DescriptorLayoutItem& layoutItem : stage)
	{
		if(layoutItem.slot == UINT_MAX)
			continue;

		if (layoutItem.type == ResourceType::SAMPLER)
		{
			outSamplerRanges.resize(outSamplerRanges.size() + 1);
			D3D12_DESCRIPTOR_RANGE1& range = outSamplerRanges[outSamplerRanges.size() - 1];

			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			range.NumDescriptors = 1;
			range.BaseShaderRegister = layoutItem.slot;
			range.RegisterSpace = layoutItem.registerSpace;
			range.OffsetInDescriptorsFromTableStart = descriptorTableSizeSamplers;
			range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

			descriptorTableSizeSamplers += 1;
		}
		else
		{
			outSrvRanges.resize(outSrvRanges.size() + 1);
			D3D12_DESCRIPTOR_RANGE1& range = outSrvRanges[outSrvRanges.size() - 1];

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
			range.OffsetInDescriptorsFromTableStart = descriptorTableSizeSRVetc;

			descriptorTableSizeSRVetc += 1;

		}
	}
}

struct D3D12DescriptorRangeStage
{
	std::vector<D3D12_DESCRIPTOR_RANGE1> samplerParams;
	std::vector<D3D12_DESCRIPTOR_RANGE1> srvParams;
	ShaderType shaderType;

	D3D12DescriptorRangeStage(ShaderType type) : shaderType(type) {}
};


D3D12DescriptorLayout::D3D12DescriptorLayout(ID3D12Device* device, const DescriptorLayoutDesc& desc) : m_desc(desc), m_rootSignature(nullptr)
{
	std::vector<D3D12DescriptorRangeStage> stageRanges
	{ 
		D3D12DescriptorRangeStage(ShaderType::Vertex),
		D3D12DescriptorRangeStage(ShaderType::Pixel),
		D3D12DescriptorRangeStage(ShaderType::AllGraphics),
	};
	GetRootRanges(desc.VS, stageRanges[0].srvParams, stageRanges[0].samplerParams);
	GetRootRanges(desc.PS, stageRanges[1].srvParams, stageRanges[1].samplerParams);
	GetRootRanges(desc.ALL, stageRanges[2].srvParams, stageRanges[2].samplerParams);

	std::vector<D3D12_ROOT_PARAMETER1> rootParameters;
	for (const auto& stageRange : stageRanges)
	{
		if (stageRange.samplerParams.size() > 0)
		{
			rootParameters.resize(rootParameters.size() + 1);
			D3D12_ROOT_PARAMETER1& param = rootParameters[rootParameters.size() - 1];

			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12ShaderVisibility(stageRange.shaderType);
			param.DescriptorTable.NumDescriptorRanges = UINT(stageRange.samplerParams.size());
			param.DescriptorTable.pDescriptorRanges = stageRange.samplerParams.data();
		}

		if (stageRange.srvParams.size() > 0)
		{
			rootParameters.resize(rootParameters.size() + 1);
			D3D12_ROOT_PARAMETER1& param = rootParameters[rootParameters.size() - 1];

			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12ShaderVisibility(stageRange.shaderType);
			param.DescriptorTable.NumDescriptorRanges = UINT(stageRange.srvParams.size());
			param.DescriptorTable.pDescriptorRanges = stageRange.srvParams.data();
		}
	}

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
	rsDesc.Desc_1_1.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	if (!rootParameters.empty())
	{
		rsDesc.Desc_1_1.pParameters = rootParameters.data();
		rsDesc.Desc_1_1.NumParameters = UINT(rootParameters.size());
	}

	Microsoft::WRL::ComPtr<ID3DBlob> rsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	VerifySuccess(D3D12SerializeVersionedRootSignature(&rsDesc, &rsBlob, &errorBlob));

	VerifySuccess(device->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
}

const INF::GFX::DescriptorLayoutDesc& D3D12DescriptorLayout::GetDesc() const
{
	return m_desc;
}

void GetRootParamtersForState(const StageBindingDescriptorDesc& stageLayoutDesc, const StageDescriptorSetDesc& stageSetDesc, std::unordered_map<DescriptorLayoutItem, uint32_t>& layoutRootIndexMap)
{
	//Search for matching slots in the layout and set
	for (int i = 0; i < MaxBindingsPerStage; ++i)
	{
		const DescriptorLayoutItem& layoutItem = stageLayoutDesc.at(i);
		if (layoutItem.slot == UINT_MAX)
			continue;

		auto it = layoutRootIndexMap.find(layoutItem);
		INF_ASSERT(it == layoutRootIndexMap.end(), std::format("Duplicate descriptor in layout with the same slot and type: slot{}, type{}", layoutItem.slot, (uint8_t)layoutItem.type));
		if (it == layoutRootIndexMap.end())
		{
			layoutRootIndexMap.emplace(layoutItem, layoutItem.slot);
		}
	}

	for (int i = 0; i < MaxBindingsPerStage; ++i)
	{
		const DescriptorSetItem& setItem = stageSetDesc.at(i);
		if (setItem.slot == UINT_MAX)
			continue;

		//check if found in layout
		DescriptorLayoutItem layoutItem{ setItem.slot, setItem.type, setItem.registerSpace };
		auto found = layoutRootIndexMap.find(layoutItem);
		INF_ASSERT(found != layoutRootIndexMap.end(), std::format("Descriptor set item not found in layout: slot{}, type{}", setItem.slot, (uint8_t)setItem.type));
		if(found == layoutRootIndexMap.end())
			continue;

	}
}

D3D12DescriptorSet::D3D12DescriptorSet(ID3D12Device* device, IDescriptorLayout* layout, const DescriptorSetDesc& setDesc) : m_desc(setDesc), m_layout(layout)
{
	INF_ASSERT(layout, "Layout can't be null");

	GetRootParamtersForState(layout->GetDesc().VS, setDesc.VS, m_layoutRootIndexMap);
	GetRootParamtersForState(layout->GetDesc().PS, setDesc.PS, m_layoutRootIndexMap);
	GetRootParamtersForState(layout->GetDesc().ALL, setDesc.ALL, m_layoutRootIndexMap);
}


uint32_t D3D12DescriptorSet::GetRootParamIndex(const DescriptorSetItem& setItem)
{
	DescriptorLayoutItem layoutItem{ setItem.slot, setItem.type, setItem.registerSpace };
	auto found = m_layoutRootIndexMap.find(layoutItem);
	INF_ASSERT(found != m_layoutRootIndexMap.end(), std::format("Descriptor set item not found in layout: slot{}, type{}", setItem.slot, (uint8_t)setItem.type));
	if (found != m_layoutRootIndexMap.end())
		return found->second;

	return UINT_MAX;
}


const DescriptorSetDesc& D3D12DescriptorSet::GetDesc() const
{
	return m_desc;
}

IDescriptorLayout* D3D12DescriptorSet::GetLayout() const
{
	return m_layout;
}
