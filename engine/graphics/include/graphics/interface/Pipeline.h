#pragma once
#include "Shader.h"
#include "graphics/GraphicDefines.h"
#include "graphics/interface/Shader.h"
#include "graphics/interface/Descriptor.h"
#include <cstdint>
#include <vector>

namespace INF::GFX
{

	struct BlendState
	{
		std::array<bool, MAX_RENDER_TARGETS>		blendEnable;
		std::array<BlendFactor, MAX_RENDER_TARGETS> srcBlend ;
		std::array<BlendFactor, MAX_RENDER_TARGETS> destBlend;
		std::array<BlendOp, MAX_RENDER_TARGETS>     blendOp;
		std::array<BlendFactor, MAX_RENDER_TARGETS> srcBlendAlpha;
		std::array<BlendFactor, MAX_RENDER_TARGETS> destBlendAlpha;
		std::array<BlendOp, MAX_RENDER_TARGETS>     blendOpAlpha;
		std::array<ColorMask, MAX_RENDER_TARGETS>   colorWriteMask;

		Color blendFactor;
		bool alphaToCoverage;
		BlendState() : blendFactor(0,0,0,0), alphaToCoverage(false)
		{
			for (int i = 0; i < MAX_RENDER_TARGETS; ++i)
			{
				blendEnable[i] = false;
				srcBlend[i] = BlendFactor::ONE;
				destBlend[i] = BlendFactor::ZERO;
				blendOp[i] = BlendOp::ADD;
				srcBlendAlpha[i] = BlendFactor::ONE;
				destBlendAlpha[i] = BlendFactor::ZERO;
				blendOpAlpha[i] = BlendOp::ADD;
				colorWriteMask[i] = ColorMask::ALL;
			}
		}
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

		DescriptorLayoutHandle descriptorLayout;

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

	using GraphicsPipelineHandle = std::shared_ptr<IGraphicsPipeline>;

	class IFramebuffer;
	class IVertexBuffer;
	class IIndexBuffer;
	class View;
	struct GraphicsState
	{
		IGraphicsPipeline* pipeline{ nullptr };
		IFramebuffer* framebuffer{ nullptr };

		//Descriptor set contains the vies to the actual GPU resources such as textures/buffers
		//Descriptor set inputs need to match the descriptor layout that was used to create the graphics pipeline
		IDescriptorSet* descriptorSet{ nullptr };

		IVertexBuffer* vertexBuffer{ nullptr };
		IIndexBuffer* indexBuffer{ nullptr };

		const View* view{ nullptr };
	};
}