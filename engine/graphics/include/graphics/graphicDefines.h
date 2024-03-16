#pragma once
#include <cstdint>

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

	struct Viewport
	{
		float X;
		float Y;
		float Width;
		float Height;
		float MinDepth;
		float MaxDepth;

		Viewport() = default;
		Viewport(float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f) :
			X(x), Y(y), Width(width), Height(height), MinDepth(minDepth), MaxDepth(maxDepth) {}
	};

	struct Rect
	{
		uint32_t Left;
		uint32_t Top;
		uint32_t Right;
		uint32_t Bottom;

		Rect() = default;
		Rect(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) : Left(left), Top(top), Right(right), Bottom(bottom) {}
	};

	enum class PrimitiveType : uint8_t
	{
		POINT_LIST,
		LINE_LIST,
		TRIANGLE_LIST,
	};

	enum class BlendFactor : uint8_t
	{
		ZERO,
		ONE,
		SRC_COLOR,
		INV_SRC_COLOR,
		SRC_ALPHA,
		INV_SRC_ALPHA,
		DST_ALPHA,
		INV_DST_ALPHA,
		DST_COLOR,
		INV_DST_COLOR ,
		SRC_ALPHA_SATURATE,
		CONSTANT_COLOR,
		INV_CONSTANT_COLOR,
		SRC1_COLOR,
		INV_SRC1_COLOR,
		SRC1_ALPHA,
		INV_SRC1_ALPHA,
	};

	enum class BlendOp : uint8_t
	{
		ADD,
		SUBRTACT,
		REVERSE_SUBTRACT,
		MIN,
		MAX
	};

	enum class ColorMask : uint8_t
	{
		RED = 1 << 0,
		GREEN = 1 << 1,
		BLUE = 1 << 2,
		ALPHA = 1 << 3,
		ALL = RED | GREEN | BLUE | ALPHA
	};

	enum class StencilOp : uint8_t
	{
		KEEP,
		ZERO,
		REPLACE,
		INCREMENT_CLAMP,
		DECREMENT_CLAMP,
		INVERT,
		INCREMENT_WRAP,
		DECREMENT_WRAP
	};

	enum class ComparisonFunc : uint8_t
	{
		NEVER,
		LESS,
		EQUAL,
		LESS_OR_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_OR_EQUAL,
		ALWAYS
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
		Vertex,
		Pixel,
		AllGraphics,

		Compute,
		All,
	};

	enum class CommandQueue : uint8_t
	{
		GRAPHICS = 0,
		COMPUTE,
		COPY,

		COUNT
	};

	enum class WrapMode : uint8_t
	{
		CLAMP,
		WRAP,
		BORDER
	};

	enum class ReductionType : uint8_t
	{
		STANDARD,
		COMPARISON,
		MINIMUM,
		MAXIMUM
	};

	constexpr uint8_t MAX_RENDER_TARGETS = 8;

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
	inline bool IsDepthFormat(Format format)
	{
		switch (format)
		{
		case Format::D16:
		case Format::D24S8:
		case Format::X24G8_UINT:
		case Format::D32:
		case Format::D32S8:
		case Format::X32G8_UINT:
			return true;
		default:
			return false;
		}
	}

	using TRANSITION_STATES_FLAGS = uint16_t;
	enum class TRANSITION_STATES : TRANSITION_STATES_FLAGS
	{
		COMMON						= 1 << 0,
		PRESENT						= 1 << 0,
		VERTEX_BUFFER				= 1 << 1,
		CONSTANT_BUFFER				= 1 << 2,
		INDEX_BUFFER				= 1 << 3,
		RENDER_TARGET				= 1 << 4,
		UNORDERED_ACCESS			= 1 << 5,
		DEPTH_WRITE					= 1 << 6,
		DEPTH_READ					= 1 << 7,
		NON_PIXEL_SHADER_RESOURCE	= 1 << 8,
		PIXEL_SHADER_RESOURCE		= 1 << 9,
		STREAM_OUT					= 1 << 10,
		COPY_DEST					= 1 << 11,
		COPY_SOURCE					= 1 << 12,
		RESOLVE_DEST				= 1 << 13,
		RESOLVE_SOURCE				= 1 << 14,
	};
	
}