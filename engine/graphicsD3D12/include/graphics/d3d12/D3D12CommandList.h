#pragma once
#include "graphics/interface/ComandList.h"
#include "graphics/graphicDefines.h"
#include "directx/d3d12.h"
#include "wrl/client.h"

namespace INF::GFX
{
	class D3D12Queue
	{
	public:
		D3D12Queue(ID3D12Device* d3dDevice, CommandQueue queueType);
		~D3D12Queue();
		void ExecuteCommandLists(ID3D12GraphicsCommandList* commandLists, uint32_t commandListCount);
		void Wait();
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> Queue;
		Microsoft::WRL::ComPtr<ID3D12Fence> Fence;
		HANDLE m_fenceEvent;
		uint64_t LastSubmittedInstance = 0;
		uint64_t LastCompletedInstance = 0;
		uint64_t UpdateLastCompletedInstance();
	};

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