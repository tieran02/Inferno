#pragma once
#include "Shader.h"
#include "graphics/GraphicDefines.h"
#include "graphics/interface/Shader.h"
#include <cstdint>
#include <vector>

namespace INF::GFX
{

	struct BlendState
	{
		bool        blendEnable = false;
		BlendFactor srcBlend = BlendFactor::ONE;
		BlendFactor destBlend = BlendFactor::ZERO;
		BlendOp     blendOp = BlendOp::ADD;
		BlendFactor srcBlendAlpha = BlendFactor::ONE;
		BlendFactor destBlendAlpha = BlendFactor::ZERO;
		BlendOp     blendOpAlpha = BlendOp::ADD;
		ColorMask   colorWriteMask = ColorMask::ALL;
	};

	struct StencilOpDesc
	{
		StencilOp failOp = StencilOp::KEEP;
		StencilOp depthFailOp = StencilOp::KEEP;
		StencilOp passOp = StencilOp::KEEP	;
		ComparisonFunc stencilFunc = ComparisonFunc::ALWAYS;
	};

	struct DepthStencilState
	{
		bool            depthTestEnable = true;
		bool            depthWriteEnable = true;
		ComparisonFunc  depthFunc = ComparisonFunc::LESS;
		bool            stencilEnable = false;
		uint8_t         stencilReadMask = 0xff;
		uint8_t         stencilWriteMask = 0xff;
		uint8_t         stencilRefValue = 0;
		StencilOpDesc   frontFaceStencil;
		StencilOpDesc   backFaceStencil;
	};

	struct RasterState
	{
		RasterFillMode fillMode = RasterFillMode::SOLID;
		RasterCullMode cullMode = RasterCullMode::BACK;
		bool frontCounterClockwise = false;
		bool depthClipEnable = false;
		bool scissorEnable = false;
		bool multisampleEnable = false;
		int depthBias = 0;
		float depthBiasClamp = 0.0f;
		float slopeScaledDepthBias = 0.0f;
	};

	struct InputElemenentDesc
	{
		std::string semanticName;
		Format format;
	};

	using InputLayoutDesc = std::vector<InputElemenentDesc>;

	struct GraphicsPipelineDesc
	{
		PrimitiveType primitiveType = PrimitiveType::TRIANGLE_LIST;
		InputLayoutDesc inputLayoutDesc;

		ShaderHandle VS;
		ShaderHandle PS;

		BlendState blendState;
		DepthStencilState depthStencilState;
		RasterState rasterState;
	};

	class IGraphicsPipeline
	{
	public:
		virtual ~IGraphicsPipeline() = default;
		virtual const GraphicsPipelineDesc& GetDesc() const = 0;
	};
}