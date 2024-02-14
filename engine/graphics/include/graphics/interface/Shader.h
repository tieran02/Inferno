#pragma once
#include "graphics/graphicDefines.h"
#include <string>
#include <memory>

namespace INF::GFX
{
	struct ShaderDesc
	{
		ShaderType shaderType = ShaderType::Vertex;
		std::string shaderPath;
		std::string entryName = "main";
	};

	class IShader
	{
	public:
		virtual ~IShader() = default;
		virtual const ShaderDesc& GetDesc() const = 0;
		virtual void GetBytecode(const void** ppBytecode, size_t& pSize) const = 0;
	};

	using ShaderHandle = std::shared_ptr<IShader>;
}