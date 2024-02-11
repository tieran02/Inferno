#pragma once
#include "graphics/interface/Pipeline.h"
#include <wrl/client.h>
#include "directx/d3d12.h"

namespace INF::GFX
{
	class IFramebuffer;
	class D3D12GraphicsPipeline : public IGraphicsPipeline
	{
	public:
		D3D12GraphicsPipeline(const GraphicsPipelineDesc& desc, Microsoft::WRL::ComPtr<ID3D12PipelineState>& pso, IFramebuffer* fb);
		const GraphicsPipelineDesc& GetDesc() const override;

		ID3D12PipelineState* PipelineState() const { return m_pso.Get(); }
		bool RequiresBlendFactor() const { return m_requiresBlendFactor; }
	private:
		GraphicsPipelineDesc m_desc;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso; //shared ref to pso, other pipeline may use the same pso
		bool m_requiresBlendFactor;
	};
}