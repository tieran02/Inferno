#include "infPCH.h"
#include "graphics/d3d12/D3D12Pipeline.h"
#include "graphics/d3d12/D3D12Texture.h"
#include "core/Assert.h"

using namespace INF::GFX;

D3D12GraphicsPipeline::D3D12GraphicsPipeline(const GraphicsPipelineDesc& desc, Microsoft::WRL::ComPtr<ID3D12PipelineState>& pso, Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature, IFramebuffer* fb) 
	: m_desc(desc)
{
	INF_ASSERT(pso, "Invalid PSO");

	m_desc = desc;
	m_pso = pso;
	m_rootSignature = rootSignature;


	for (size_t index = 0; index < fb->GetInfo().colorFormats.size(); index++)
	{
		if (desc.blendState.srcBlend[index] == BlendFactor::CONSTANT_COLOR || desc.blendState.srcBlend[index] == BlendFactor::INV_CONSTANT_COLOR ||
			desc.blendState.destBlend[index] == BlendFactor::CONSTANT_COLOR || desc.blendState.destBlend[index] == BlendFactor::INV_CONSTANT_COLOR ||
			desc.blendState.srcBlendAlpha[index] == BlendFactor::CONSTANT_COLOR || desc.blendState.srcBlendAlpha[index] == BlendFactor::INV_CONSTANT_COLOR ||
			desc.blendState.destBlendAlpha[index] == BlendFactor::CONSTANT_COLOR || desc.blendState.destBlendAlpha[index] == BlendFactor::INV_CONSTANT_COLOR)
		{
			m_requiresBlendFactor = true;
		}
	}
}

const GraphicsPipelineDesc& D3D12GraphicsPipeline::GetDesc() const
{
	return m_desc;
}
