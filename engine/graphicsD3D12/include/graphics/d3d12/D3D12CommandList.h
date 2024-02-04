#pragma once
#include "graphics/interface/ComandList.h"
#include "graphics/graphicDefines.h"
#include "directx/d3d12.h"
#include "wrl/client.h"

namespace INF::GFX
{
	class D3D12CommandList : public ICommandList
	{
	public:
		D3D12CommandList(ID3D12Device* d3dDevice, ID3D12CommandAllocator* commandAllocator, CommandQueue queueType);
		void Open() override;
		void Close() override;
	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	};
}