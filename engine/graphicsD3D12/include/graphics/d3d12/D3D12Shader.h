#pragma once
#include "graphics/interface/Shader.h"
#include "directx/d3d12.h"

namespace INF::GFX
{
	class D3D12Shader : public IShader
	{
	public:
		D3D12Shader(const ShaderDesc& shaderDesc);

		const ShaderDesc& GetDesc() const override;
		void GetBytecode(const void** ppBytecode, size_t& pSize) const override;
		D3D12_SHADER_BYTECODE D3D();
	private:
		ShaderDesc m_shaderDesc;
		std::vector<char> m_byteCode;
	};
}