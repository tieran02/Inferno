#pragma once
#include "graphics/interface/Pipeline.h"

namespace INF::GFX
{
	class IFramebuffer;
	class D3D12GraphicsPipeline : public IGraphicsPipeline
	{
	public:
		D3D12GraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb);
		const GraphicsPipelineDesc& GetDesc() const override;
	private:
		GraphicsPipelineDesc m_desc;
	};
}