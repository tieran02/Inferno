#include "infPCH.h"
#include "graphics/d3d12/D3D12Pipeline.h"

using namespace INF::GFX;

D3D12GraphicsPipeline::D3D12GraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb) : m_desc(desc)
{

}

const GraphicsPipelineDesc& D3D12GraphicsPipeline::GetDesc() const
{
	return m_desc;
}
