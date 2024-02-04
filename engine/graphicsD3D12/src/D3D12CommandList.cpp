#include "infPCH.h"
#include "graphics/d3d12/D3D12CommandList.h"
#include "graphics/d3d12/D3D12Defines.h"

namespace INF::GFX
{

	D3D12CommandList::D3D12CommandList(ID3D12Device* d3dDevice, ID3D12CommandAllocator* commandAllocator, CommandQueue queueType)
	{
		switch (queueType)
		{
		case INF::GFX::CommandQueue::GRAPHICS:
			VerifySuccess(d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&m_commandList)));
			break;
		case INF::GFX::CommandQueue::COMPUTE:
			throw std::logic_error("The method or operation is not implemented.");
			break;
		case INF::GFX::CommandQueue::COPY:
			throw std::logic_error("The method or operation is not implemented.");
			break;
		default:
			throw std::logic_error("The method or operation is not implemented.");
			break;
		}
	}

	void D3D12CommandList::Open()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void D3D12CommandList::Close()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}