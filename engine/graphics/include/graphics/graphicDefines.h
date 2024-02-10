#pragma once

namespace INF::GFX
{
	enum class API
	{
		D3D12,

		COUNT
	};

	struct Color
	{
		float R, G, B, A;
		Color() : R(0.f), G(0.f), B(0.f), A(0.f) { }
		Color(float c) : R(c), G(c), B(c), A(c) { }
		Color(float r, float g, float b, float a) : R(r), G(g), B(b), A(a) { }

		bool operator ==(const Color& rhs) const { return R == rhs.R && G == rhs.G && B == rhs.B && A == rhs.A; }
		bool operator !=(const Color& rhs) const { return R != rhs.R || G != rhs.G || B != rhs.B || A != rhs.A; }
	};

	enum class PrimitiveType : uint8_t
	{
		POINT_LIST,
		LINE_LIST,
		TRIANGLE_LIST,
		TRIANGLE_STRIP,
	};

	enum class BlendFactor : uint8_t
	{
		ZERO = 1,
		ONE = 2,
		SRC_COLOR = 3,
		INV_SRC_COLOR = 4,
		SRC_ALPHA = 5,
		INV_SRC_ALPHA = 6,
		DST_ALPHA = 7,
		INV_DST_ALPHA = 8,
		DST_COLOR = 9,
		INV_DST_COLOR = 10,
		SRC_ALPHA_SATURATE = 11,
		CONSTANT_COLOR = 14,
		INV_CONSTANT_COLOR = 15,
		SRC1_COLOR = 16,
		INV_SRC1_COLOR = 17,
		SRC1_ALPHA = 18,
		INV_SRC1_ALPHA = 19,
	};

	enum class BlendOp : uint8_t
	{
		ADD = 1,
		SUBRTACT = 2,
		REVERSE_SUBTRACT = 3,
		MIN = 4,
		MAX = 5
	};

	enum class ColorMask : uint8_t
	{
		RED = 1,
		GREEN = 2,
		BLUE = 4,
		ALPHA = 8,
		ALL = 0XF
	};

	enum class StencilOp : uint8_t
	{
		KEEP = 1,
		ZERO = 2,
		REPLACE = 3,
		INCREMENT_CLAMP = 4,
		DECREMENT_CLAMP = 5,
		INVERT = 6,
		INCREMENT_WRAP = 7,
		DECREMENT_WRAP = 8
	};

	enum class ComparisonFunc : uint8_t
	{
		NEVER = 1,
		LESS = 2,
		EQUAL = 3,
		LESS_OR_EQUAL = 4,
		GREATER = 5,
		NOT_EQUAL = 6,
		GREATER_OR_EQUAL = 7,
		ALWAYS = 8
	};

	enum class RasterFillMode : uint8_t
	{
		SOLID,
		WIREFRAME,
	};

	enum class RasterCullMode : uint8_t
	{
		BACK,
		FRONT,
		NONE
	};

	enum class ShaderType : uint8_t
	{
		None = 1 << 0,

		Vertex = 1 << 1,
		Pixel = 1 << 2,
		AllGraphics = 1 << 3,

		Compute = 1 << 4,
		All = 0xff,
	};

	enum class CommandQueue : uint8_t
	{
		GRAPHICS = 0,
		COMPUTE,
		COPY,

		COUNT
	};


	enum class Format
	{
		UNKNOWN,

		R8_UINT,
		R8_SINT,
		R8_UNORM,
		R8_SNORM,
		RG8_UINT,
		RG8_SINT,
		RG8_UNORM,
		RG8_SNORM,
		R16_UINT,
		R16_SINT,
		R16_UNORM,
		R16_SNORM,
		R16_FLOAT,
		BGRA4_UNORM,
		B5G6R5_UNORM,
		B5G5R5A1_UNORM,
		RGBA8_UINT,
		RGBA8_SINT,
		RGBA8_UNORM,
		RGBA8_SNORM,
		BGRA8_UNORM,
		SRGBA8_UNORM,
		SBGRA8_UNORM,
		R10G10B10A2_UNORM,
		R11G11B10_FLOAT,
		RG16_UINT,
		RG16_SINT,
		RG16_UNORM,
		RG16_SNORM,
		RG16_FLOAT,
		R32_UINT,
		R32_SINT,
		R32_FLOAT,
		RGBA16_UINT,
		RGBA16_SINT,
		RGBA16_FLOAT,
		RGBA16_UNORM,
		RGBA16_SNORM,
		RG32_UINT,
		RG32_SINT,
		RG32_FLOAT,
		RGB32_UINT,
		RGB32_SINT,
		RGB32_FLOAT,
		RGBA32_UINT,
		RGBA32_SINT,
		RGBA32_FLOAT,

		// be sure to adjust utility functions below if adding or rearranging depthstencil formats
		D16,
		D24S8,
		X24G8_UINT,
		D32,
		D32S8,
		X32G8_UINT,

		BC1_UNORM,
		BC1_UNORM_SRGB,
		BC2_UNORM,
		BC2_UNORM_SRGB,
		BC3_UNORM,
		BC3_UNORM_SRGB,
		BC4_UNORM,
		BC4_SNORM,
		BC5_UNORM,
		BC5_SNORM,
		BC6H_UFLOAT,
		BC6H_SFLOAT,
		BC7_UNORM,
		BC7_UNORM_SRGB,

		COUNT,
	};

	using TRANSITION_STATES_FLAGS = uint16_t;
	enum class TRANSITION_STATES : TRANSITION_STATES_FLAGS
	{
		PRESENT						= 1 << 0,
		INDEX_BUFFER				= 1 << 1,
		RENDER_TARGET				= 1 << 2,
		UNORDERED_ACCESS			= 1 << 3,
		DEPTH_WRITE					= 1 << 4,
		DEPTH_READ					= 1 << 5,
		NON_PIXEL_SHADER_RESOURCE	= 1 << 6,
		PIXEL_SHADER_RESOURCE		= 1 << 7,
		STREAM_OUT					= 1 << 8,
		COPY_DEST					= 1 << 9,
		COPY_SOURCE					= 1 << 10,
		RESOLVE_DEST				= 1 << 11,
		RESOLVE_SOURCE				= 1 << 12,
	};
}