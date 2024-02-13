#pragma once
#include "core/Assert.h"
#include "graphics/graphicDefines.h"
#include "graphics/interface/Pipeline.h"

#define VerifySuccess(hr) INF_VERIFY(SUCCEEDED(hr), "D3D12 Call did not succeed");


inline DXGI_FORMAT D3D12Format(INF::GFX::Format format)
{
	switch (format)
	{
	case INF::GFX::Format::UNKNOWN:
		return DXGI_FORMAT_UNKNOWN;
	case INF::GFX::Format::R8_UINT:
		return DXGI_FORMAT_R8_UINT;
	case INF::GFX::Format::R8_SINT:
		return DXGI_FORMAT_R8_SINT;
	case INF::GFX::Format::R8_UNORM:
		return DXGI_FORMAT_R8_UNORM;
	case INF::GFX::Format::R8_SNORM:
		return DXGI_FORMAT_R8_SNORM;
	case INF::GFX::Format::RG8_UINT:
		return DXGI_FORMAT_R8G8_UINT;
	case INF::GFX::Format::RG8_SINT:
		return DXGI_FORMAT_R8G8_SINT;
	case INF::GFX::Format::RG8_UNORM:
		return DXGI_FORMAT_R8G8_UNORM;
	case INF::GFX::Format::RG8_SNORM:
		return DXGI_FORMAT_R8G8_SNORM;
	case INF::GFX::Format::R16_UINT:
		return DXGI_FORMAT_R16_UINT;
	case INF::GFX::Format::R16_SINT:
		return DXGI_FORMAT_R16_SINT;
	case INF::GFX::Format::R16_UNORM:
		return DXGI_FORMAT_R16_UNORM;
	case INF::GFX::Format::R16_SNORM:
		return DXGI_FORMAT_R16_SNORM;
	case INF::GFX::Format::R16_FLOAT:
		return DXGI_FORMAT_R16_FLOAT;
	case INF::GFX::Format::BGRA4_UNORM:
		return DXGI_FORMAT_B4G4R4A4_UNORM;
	case INF::GFX::Format::B5G6R5_UNORM:
		return DXGI_FORMAT_B5G6R5_UNORM;
	case INF::GFX::Format::B5G5R5A1_UNORM:
		return DXGI_FORMAT_B5G5R5A1_UNORM;
	case INF::GFX::Format::RGBA8_UINT:
		return DXGI_FORMAT_R8G8B8A8_UINT;
	case INF::GFX::Format::RGBA8_SINT:
		return DXGI_FORMAT_R8G8B8A8_SINT;
	case INF::GFX::Format::RGBA8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case INF::GFX::Format::RGBA8_SNORM:
		return DXGI_FORMAT_R8G8B8A8_SNORM;
	case INF::GFX::Format::BGRA8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case INF::GFX::Format::SRGBA8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case INF::GFX::Format::SBGRA8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	case INF::GFX::Format::R10G10B10A2_UNORM:
		return DXGI_FORMAT_R10G10B10A2_UNORM;
	case INF::GFX::Format::R11G11B10_FLOAT:
		return DXGI_FORMAT_R11G11B10_FLOAT;
	case INF::GFX::Format::RG16_UINT:
		return DXGI_FORMAT_R16G16_UINT;
	case INF::GFX::Format::RG16_SINT:
		return DXGI_FORMAT_R16G16_SINT;
	case INF::GFX::Format::RG16_UNORM:
		return DXGI_FORMAT_R16G16_UNORM;
	case INF::GFX::Format::RG16_SNORM:
		return DXGI_FORMAT_R16G16_SNORM;
	case INF::GFX::Format::RG16_FLOAT:
		return DXGI_FORMAT_R16G16_FLOAT;
	case INF::GFX::Format::R32_UINT:
		return DXGI_FORMAT_R32_UINT;
	case INF::GFX::Format::R32_SINT:
		return DXGI_FORMAT_R32_SINT;
	case INF::GFX::Format::R32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
	case INF::GFX::Format::RGBA16_UINT:
		return DXGI_FORMAT_R16G16B16A16_UINT;
	case INF::GFX::Format::RGBA16_SINT:
		return DXGI_FORMAT_R16G16B16A16_SINT;
	case INF::GFX::Format::RGBA16_FLOAT:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case INF::GFX::Format::RGBA16_UNORM:
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	case INF::GFX::Format::RGBA16_SNORM:
		return DXGI_FORMAT_R16G16B16A16_SNORM;
	case INF::GFX::Format::RG32_UINT:
		return DXGI_FORMAT_R32G32_UINT;
	case INF::GFX::Format::RG32_SINT:
		return DXGI_FORMAT_R32G32_SINT;
	case INF::GFX::Format::RG32_FLOAT:
		return DXGI_FORMAT_R32G32_FLOAT;
	case INF::GFX::Format::RGB32_UINT:
		return DXGI_FORMAT_R32G32_UINT;
	case INF::GFX::Format::RGB32_SINT:
		return DXGI_FORMAT_R32G32_SINT;
	case INF::GFX::Format::RGB32_FLOAT:
		return DXGI_FORMAT_R32G32_FLOAT;
	case INF::GFX::Format::RGBA32_UINT:
		return DXGI_FORMAT_R32G32B32A32_UINT;
	case INF::GFX::Format::RGBA32_SINT:
		return DXGI_FORMAT_R32G32B32A32_SINT;
	case INF::GFX::Format::RGBA32_FLOAT:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case INF::GFX::Format::D16:
		return DXGI_FORMAT_D16_UNORM;
	case INF::GFX::Format::D24S8:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case INF::GFX::Format::X24G8_UINT:
		return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
	case INF::GFX::Format::D32:
		return DXGI_FORMAT_D32_FLOAT;
	case INF::GFX::Format::D32S8:
		return DXGI_FORMAT_UNKNOWN; //not supported?
	case INF::GFX::Format::X32G8_UINT:
		return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
	case INF::GFX::Format::BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM;
	case INF::GFX::Format::BC1_UNORM_SRGB:
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	case INF::GFX::Format::BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM;
	case INF::GFX::Format::BC2_UNORM_SRGB:
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	case INF::GFX::Format::BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM;
	case INF::GFX::Format::BC3_UNORM_SRGB:
		return DXGI_FORMAT_BC3_UNORM_SRGB;
	case INF::GFX::Format::BC4_UNORM:
		return DXGI_FORMAT_BC4_UNORM;
	case INF::GFX::Format::BC4_SNORM:
		return DXGI_FORMAT_BC4_SNORM;
	case INF::GFX::Format::BC5_UNORM:
		return DXGI_FORMAT_BC5_UNORM;
	case INF::GFX::Format::BC5_SNORM:
		return DXGI_FORMAT_BC5_SNORM;
	case INF::GFX::Format::BC6H_UFLOAT:
		return DXGI_FORMAT_BC6H_UF16;
	case INF::GFX::Format::BC6H_SFLOAT:
		return DXGI_FORMAT_BC6H_SF16;
	case INF::GFX::Format::BC7_UNORM:
		return DXGI_FORMAT_BC7_UNORM;
	case INF::GFX::Format::BC7_UNORM_SRGB:
		return DXGI_FORMAT_BC7_UNORM_SRGB;
	default:
		return DXGI_FORMAT_UNKNOWN;
	}
}

inline D3D12_RESOURCE_STATES D3D12TransitionFlags(INF::GFX::TRANSITION_STATES_FLAGS flags)
{
	D3D12_RESOURCE_STATES d3dFlags = D3D12_RESOURCE_STATE_COMMON;

	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::PRESENT)
		d3dFlags |= D3D12_RESOURCE_STATE_PRESENT;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::VERTEX_BUFFER)
		d3dFlags |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::CONSTANT_BUFFER)
		d3dFlags |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	if(flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::INDEX_BUFFER)
		d3dFlags |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::RENDER_TARGET)
		d3dFlags |= D3D12_RESOURCE_STATE_RENDER_TARGET;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::UNORDERED_ACCESS)
		d3dFlags |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::DEPTH_WRITE)
		d3dFlags |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::DEPTH_READ)
		d3dFlags |= D3D12_RESOURCE_STATE_DEPTH_READ;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::NON_PIXEL_SHADER_RESOURCE)
		d3dFlags |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::PIXEL_SHADER_RESOURCE)
		d3dFlags |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::STREAM_OUT)
		d3dFlags |= D3D12_RESOURCE_STATE_STREAM_OUT;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::COPY_DEST)
		d3dFlags |= D3D12_RESOURCE_STATE_COPY_DEST;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::COPY_SOURCE)
		d3dFlags |= D3D12_RESOURCE_STATE_COPY_SOURCE;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::RESOLVE_DEST)
		d3dFlags |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
	if (flags & (INF::GFX::TRANSITION_STATES_FLAGS)INF::GFX::TRANSITION_STATES::RESOLVE_SOURCE)
		d3dFlags |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;

	return d3dFlags;
}

inline D3D12_BLEND D3D12Blend(INF::GFX::BlendFactor factor)
{
	switch (factor)
	{
	case INF::GFX::BlendFactor::ZERO:
		return D3D12_BLEND::D3D12_BLEND_ZERO;
	case INF::GFX::BlendFactor::ONE:
		return D3D12_BLEND::D3D12_BLEND_ONE;
	case INF::GFX::BlendFactor::SRC_COLOR:
		return D3D12_BLEND::D3D12_BLEND_SRC_COLOR;
	case INF::GFX::BlendFactor::INV_SRC_COLOR:
		return D3D12_BLEND::D3D12_BLEND_SRC_COLOR;
	case INF::GFX::BlendFactor::SRC_ALPHA:
		return D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
	case INF::GFX::BlendFactor::INV_SRC_ALPHA:
		return D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
	case INF::GFX::BlendFactor::DST_ALPHA:
		return D3D12_BLEND::D3D12_BLEND_DEST_ALPHA;
	case INF::GFX::BlendFactor::INV_DST_ALPHA:
		return D3D12_BLEND::D3D12_BLEND_INV_DEST_ALPHA;
	case INF::GFX::BlendFactor::DST_COLOR:
		return D3D12_BLEND::D3D12_BLEND_DEST_COLOR;
	case INF::GFX::BlendFactor::INV_DST_COLOR:
		return D3D12_BLEND::D3D12_BLEND_INV_DEST_ALPHA;
	case INF::GFX::BlendFactor::SRC_ALPHA_SATURATE:
		return D3D12_BLEND::D3D12_BLEND_SRC_ALPHA_SAT;
	case INF::GFX::BlendFactor::CONSTANT_COLOR:
		return D3D12_BLEND::D3D12_BLEND_BLEND_FACTOR;
	case INF::GFX::BlendFactor::INV_CONSTANT_COLOR:
		return D3D12_BLEND::D3D12_BLEND_INV_BLEND_FACTOR;
	case INF::GFX::BlendFactor::SRC1_COLOR:
		return D3D12_BLEND::D3D12_BLEND_INV_SRC1_COLOR;
	case INF::GFX::BlendFactor::INV_SRC1_COLOR:
		return D3D12_BLEND::D3D12_BLEND_INV_SRC1_COLOR;
	case INF::GFX::BlendFactor::SRC1_ALPHA:
		return D3D12_BLEND::D3D12_BLEND_SRC1_ALPHA;
	case INF::GFX::BlendFactor::INV_SRC1_ALPHA:
		return D3D12_BLEND::D3D12_BLEND_INV_SRC1_ALPHA;
	default:
		INF_ASSERT(false, "Blend factor not supported");
		return D3D12_BLEND::D3D12_BLEND_ONE;
	}
}

inline D3D12_BLEND_OP D3D12BlendOp(INF::GFX::BlendOp op)
{
	switch (op)
	{
	case INF::GFX::BlendOp::ADD:
		return D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
	case INF::GFX::BlendOp::SUBRTACT:
		return D3D12_BLEND_OP::D3D12_BLEND_OP_REV_SUBTRACT;
	case INF::GFX::BlendOp::REVERSE_SUBTRACT:
		return D3D12_BLEND_OP::D3D12_BLEND_OP_REV_SUBTRACT;
	case INF::GFX::BlendOp::MIN:
		return D3D12_BLEND_OP::D3D12_BLEND_OP_MAX;
	case INF::GFX::BlendOp::MAX:
		return D3D12_BLEND_OP::D3D12_BLEND_OP_MAX;
	default:
		INF_ASSERT(false, "Blend factor not supported");
		return D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
	}
}

inline D3D12_BLEND_DESC D3D12BlendState(INF::GFX::BlendState state)
{
	D3D12_BLEND_DESC blend;

	blend.AlphaToCoverageEnable = state.alphaToCoverage;
	blend.IndependentBlendEnable = true;

	for (uint32_t i = 0; i < INF::GFX::MAX_RENDER_TARGETS; i++)
	{
		blend.RenderTarget[i].LogicOpEnable = false;
		blend.RenderTarget[i].BlendEnable = state.blendEnable[i];
		blend.RenderTarget[i].SrcBlend = D3D12Blend(state.srcBlend[i]);
		blend.RenderTarget[i].DestBlend = D3D12Blend(state.destBlend[i]);
		blend.RenderTarget[i].BlendOp = D3D12BlendOp(state.blendOp[i]);
		blend.RenderTarget[i].SrcBlendAlpha = D3D12Blend(state.srcBlendAlpha[i]);
		blend.RenderTarget[i].DestBlendAlpha = D3D12Blend(state.destBlendAlpha[i]);
		blend.RenderTarget[i].BlendOpAlpha = D3D12BlendOp(state.blendOpAlpha[i]);
		blend.RenderTarget[i].RenderTargetWriteMask =
			((uint8_t)state.colorWriteMask[i] & (uint8_t)INF::GFX::ColorMask::RED ? D3D12_COLOR_WRITE_ENABLE_RED : 0) |
			((uint8_t)state.colorWriteMask[i] & (uint8_t)INF::GFX::ColorMask::GREEN ? D3D12_COLOR_WRITE_ENABLE_GREEN : 0) |
			((uint8_t)state.colorWriteMask[i] & (uint8_t)INF::GFX::ColorMask::BLUE ? D3D12_COLOR_WRITE_ENABLE_BLUE : 0) |
			((uint8_t)state.colorWriteMask[i] & (uint8_t)INF::GFX::ColorMask::ALPHA ? D3D12_COLOR_WRITE_ENABLE_ALPHA : 0);
	}

	return blend;
}

inline D3D12_COMPARISON_FUNC D3D12ComparisionFunc(INF::GFX::ComparisonFunc func)
{
	switch (func)
	{
	case INF::GFX::ComparisonFunc::NEVER:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
	case INF::GFX::ComparisonFunc::LESS:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
	case INF::GFX::ComparisonFunc::EQUAL:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL;
	case INF::GFX::ComparisonFunc::LESS_OR_EQUAL:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
	case INF::GFX::ComparisonFunc::GREATER:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER;
	case INF::GFX::ComparisonFunc::NOT_EQUAL:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NOT_EQUAL;
	case INF::GFX::ComparisonFunc::GREATER_OR_EQUAL:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	case INF::GFX::ComparisonFunc::ALWAYS:
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;
	default:
		INF_ASSERT(false, "Comparison Func not supported");
		return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
	}
}

inline D3D12_STENCIL_OP D3D12StencilOp(INF::GFX::StencilOp op)
{
	switch (op)
	{
	case INF::GFX::StencilOp::KEEP:
		return D3D12_STENCIL_OP::D3D12_STENCIL_OP_KEEP;
	case INF::GFX::StencilOp::ZERO:
		return D3D12_STENCIL_OP::D3D12_STENCIL_OP_ZERO;
	case INF::GFX::StencilOp::REPLACE:
		return D3D12_STENCIL_OP::D3D12_STENCIL_OP_REPLACE;
	case INF::GFX::StencilOp::INCREMENT_CLAMP:
		return D3D12_STENCIL_OP::D3D12_STENCIL_OP_INCR_SAT;
	case INF::GFX::StencilOp::DECREMENT_CLAMP:
		return D3D12_STENCIL_OP::D3D12_STENCIL_OP_DECR_SAT;
	case INF::GFX::StencilOp::INVERT:
		return D3D12_STENCIL_OP::D3D12_STENCIL_OP_INVERT;
	case INF::GFX::StencilOp::INCREMENT_WRAP:
		return D3D12_STENCIL_OP::D3D12_STENCIL_OP_INCR;
	case INF::GFX::StencilOp::DECREMENT_WRAP:
		return D3D12_STENCIL_OP::D3D12_STENCIL_OP_DECR;
	default:
		INF_ASSERT(false, "Stencil Op not supported");
		return D3D12_STENCIL_OP::D3D12_STENCIL_OP_KEEP;
	}
}

inline D3D12_DEPTH_STENCIL_DESC D3D12DepthStencilState(const INF::GFX::DepthStencilState& state)
{
	D3D12_DEPTH_STENCIL_DESC depthStencil;

	depthStencil.DepthEnable = state.depthTestEnable;
	depthStencil.DepthWriteMask = state.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencil.DepthFunc = D3D12ComparisionFunc(state.depthFunc);
	depthStencil.StencilEnable = state.stencilEnable;
	depthStencil.StencilReadMask = state.stencilReadMask;
	depthStencil.StencilWriteMask = state.stencilWriteMask;
	depthStencil.FrontFace.StencilFailOp = D3D12StencilOp(state.frontFaceStencil.failOp);
	depthStencil.FrontFace.StencilDepthFailOp = D3D12StencilOp(state.frontFaceStencil.depthFailOp);
	depthStencil.FrontFace.StencilPassOp = D3D12StencilOp(state.frontFaceStencil.passOp);
	depthStencil.FrontFace.StencilFunc = D3D12ComparisionFunc(state.frontFaceStencil.stencilFunc);
	depthStencil.BackFace.StencilFailOp = D3D12StencilOp(state.backFaceStencil.failOp);
	depthStencil.BackFace.StencilDepthFailOp = D3D12StencilOp(state.backFaceStencil.depthFailOp);
	depthStencil.BackFace.StencilPassOp = D3D12StencilOp(state.backFaceStencil.passOp);
	depthStencil.BackFace.StencilFunc = D3D12ComparisionFunc(state.backFaceStencil.stencilFunc);

	return depthStencil;
}

inline D3D12_PRIMITIVE_TOPOLOGY D3D12Primitive(INF::GFX::PrimitiveType type)
{
	switch (type)
	{
	case INF::GFX::PrimitiveType::POINT_LIST:
		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case INF::GFX::PrimitiveType::LINE_LIST:
		return  D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	case INF::GFX::PrimitiveType::TRIANGLE_LIST:
		return  D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	default:
		INF_ASSERT(false, "Primitive type not supported");
		return  D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
}

inline D3D12_PRIMITIVE_TOPOLOGY_TYPE D3D12PrimitiveType(INF::GFX::PrimitiveType type)
{
	switch (type)
	{
	case INF::GFX::PrimitiveType::POINT_LIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	case INF::GFX::PrimitiveType::LINE_LIST:
		return  D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	case INF::GFX::PrimitiveType::TRIANGLE_LIST:
		return  D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	default:
		INF_ASSERT(false, "Primitive type not supported");
		return  D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}
}

inline D3D12_FILL_MODE D3D12FillMode(INF::GFX::RasterFillMode fillMode)
{
	switch (fillMode)
	{
	case INF::GFX::RasterFillMode::SOLID:
		return D3D12_FILL_MODE_SOLID;
	case INF::GFX::RasterFillMode::WIREFRAME:
		return D3D12_FILL_MODE_WIREFRAME;
	default:
		INF_ASSERT(false, "Raster Fill Mode not supported");
		return D3D12_FILL_MODE_SOLID;
	}
}

inline D3D12_RASTERIZER_DESC D3D12RasterizerState(const INF::GFX::RasterState state)
{
	D3D12_RASTERIZER_DESC rast;

	rast.FillMode = D3D12FillMode(state.fillMode);

	switch (state.cullMode)
	{
	case INF::GFX::RasterCullMode::BACK:
		rast.CullMode = D3D12_CULL_MODE_BACK;
		break;
	case INF::GFX::RasterCullMode::FRONT:
		rast.CullMode = D3D12_CULL_MODE_FRONT;
		break;
	case INF::GFX::RasterCullMode::NONE:
		rast.CullMode = D3D12_CULL_MODE_NONE;
		break;
	default:
		INF_ASSERT(false, "Raster Cull Mode not supported");
	}

	rast.FrontCounterClockwise = state.frontCounterClockwise;
	rast.DepthBias = state.depthBias;
	rast.DepthBiasClamp = state.depthBiasClamp;
	rast.SlopeScaledDepthBias = state.slopeScaledDepthBias;
	rast.DepthClipEnable = state.depthClipEnable;
	rast.MultisampleEnable = state.multisampleEnable;
	rast.AntialiasedLineEnable = false;
	rast.ForcedSampleCount = 0;
	rast.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return rast;
}