#include "infPCH.h"
#include "graphics/d3d12/D3D12Shader.h"
#include "core/Assert.h"
#include "core/File.h"

namespace INF::GFX 
{
	D3D12Shader::D3D12Shader(const ShaderDesc& shaderDesc) : m_shaderDesc(shaderDesc)
	{
		INF_ASSERT(!shaderDesc.shaderPath.empty(), "No shader path set");
		File shaderFile;

		bool result = shaderFile.Open(shaderDesc.shaderPath);
		INF_ASSERT(result, std::format("Failed to load file: {}", shaderDesc.shaderPath));

		if (result)
		{
			m_byteCode = shaderFile.Contents();
		}
	}

	const ShaderDesc& D3D12Shader::GetDesc() const
	{
		return m_shaderDesc;
	}

	void D3D12Shader::GetBytecode(const void** ppBytecode, size_t& pSize) const
	{
		*ppBytecode = m_byteCode.data();
		pSize = m_byteCode.size();
	}

	D3D12_SHADER_BYTECODE D3D12Shader::D3D()
	{
		D3D12_SHADER_BYTECODE byteCode;
		byteCode.pShaderBytecode = m_byteCode.data();
		byteCode.BytecodeLength = m_byteCode.size();
		return byteCode;
	}

}
