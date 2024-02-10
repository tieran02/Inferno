#include "infPCH.h"
#include "graphics/d3d12/D3D12CommandList.h"
#include "graphics/d3d12/D3D12Defines.h"

namespace INF::GFX
{
	D3D12Queue::D3D12Queue(ID3D12Device* d3dDevice, CommandQueue queueType)
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		switch (queueType)
		{
		case INF::GFX::CommandQueue::GRAPHICS:
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
		case INF::GFX::CommandQueue::COMPUTE:
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			break;
		case INF::GFX::CommandQueue::COPY:
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			break;
		default:
			throw std::logic_error("The method or operation is not implemented.");
			break;
		}
		VerifySuccess(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&Queue)));

		VerifySuccess(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));

		m_fenceEvent = CreateEvent(nullptr, false, false, nullptr); //This can probably be within the device?
	}

	D3D12Queue::~D3D12Queue()
	{
		if (m_fenceEvent)
		{
			CloseHandle(m_fenceEvent);
			m_fenceEvent = nullptr;
		}
	}

	uint64_t D3D12Queue::ExecuteCommandLists(ID3D12GraphicsCommandList* commandLists, uint32_t commandListCount)
	{
		ID3D12CommandList* ppCommandLists[] = { static_cast<ID3D12CommandList*>(commandLists) };
		Queue->ExecuteCommandLists(commandListCount, ppCommandLists);
		LastSubmittedInstance++;
		Queue->Signal(Fence.Get(), LastSubmittedInstance);

		return LastSubmittedInstance;
	}

	void D3D12Queue::Wait()
	{
		if (UpdateLastCompletedInstance() < LastSubmittedInstance)
		{
			// Test if the fence has been reached
			if (Fence->GetCompletedValue() < LastSubmittedInstance)
			{
				// If it's not, wait for it to finish using an event
				ResetEvent(m_fenceEvent);
				Fence->SetEventOnCompletion(LastSubmittedInstance, m_fenceEvent);
				WaitForSingleObject(m_fenceEvent, INFINITE);
			}
		}
	}

	uint64_t D3D12Queue::UpdateLastCompletedInstance()
	{
		if (LastCompletedInstance < LastSubmittedInstance)
		{
			LastCompletedInstance = Fence->GetCompletedValue();
		}
		return LastCompletedInstance;
	}

	D3D12CommandList::D3D12CommandList(ID3D12Device* d3dDevice, const Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& commandAllocator, CommandQueue queueType)
	{
		m_commandAllocator = commandAllocator;
		switch (queueType)
		{
		case INF::GFX::CommandQueue::GRAPHICS:
			VerifySuccess(d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
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

		//Command lists start closed
		Close();
	}

	void D3D12CommandList::Open()
	{
		m_commandList->Reset(m_commandAllocator.Get(), nullptr);
	}

	void D3D12CommandList::Close()
	{
		m_commandList->Close();
	}
}