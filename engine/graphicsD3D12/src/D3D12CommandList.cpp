#include "infPCH.h"
#include "graphics/d3d12/D3D12CommandList.h"
#include "graphics/d3d12/D3D12Defines.h"
#include "graphics/interface/Texture.h"
#include "graphics/d3d12/D3D12Texture.h"
#include "graphics/d3d12/D3D12Pipeline.h"
#include "graphics/d3d12/D3D12Buffer.h"

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

	void D3D12CommandList::Transition(ITexture* texture, TRANSITION_STATES_FLAGS from, TRANSITION_STATES_FLAGS to)
	{
		D3D12_RESOURCE_BARRIER renderTargetBarrier;
		renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

		renderTargetBarrier.Transition.pResource = static_cast<D3D12Texture*>(texture)->Resource();
		renderTargetBarrier.Transition.StateBefore = D3D12TransitionFlags(from);
		renderTargetBarrier.Transition.StateAfter = D3D12TransitionFlags(to);
		renderTargetBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		m_commandList->ResourceBarrier(1, &renderTargetBarrier);
	}

	void D3D12CommandList::ClearColor(ITexture* texture, const Color& color)
	{
		const D3D12TextureView* textureView = (const D3D12TextureView*)texture->GetView(ITextureView::ViewType::RENDER_TARGET);
		INF_ASSERT(textureView, "Invalid Texture view");
		m_commandList->ClearRenderTargetView(textureView->CPU, &color.R, 0, nullptr);
	}

	void D3D12CommandList::SetGraphicsState(const GraphicsState& state)
	{
		D3D12GraphicsPipeline* pso = static_cast<D3D12GraphicsPipeline*>(state.pipeline);
		D3D12Framebuffer* framebuffer = static_cast<D3D12Framebuffer*>(state.framebuffer);

		BindGraphicsPipeline(pso);
		BindFramebuffer(pso, framebuffer);

		D3D12VertexBuffer* vertexBuffer = static_cast<D3D12VertexBuffer*>(state.vertexBuffer);
		m_commandList->IASetVertexBuffers(0,1, &vertexBuffer->View());

		D3D12IndexBuffer* indexBuffer = static_cast<D3D12IndexBuffer*>(state.indexBuffer);
		m_commandList->IASetIndexBuffer(&indexBuffer->View());
	}

	void D3D12CommandList::SetViewport(const Viewport& viewport)
	{
		m_commandList->RSSetViewports(1, reinterpret_cast<const D3D12_VIEWPORT*>(&viewport));
	}

	void D3D12CommandList::SetScissor(const Rect& scissor)
	{
		m_commandList->RSSetScissorRects(1, reinterpret_cast<const D3D12_RECT*>(&scissor));
	}

	void D3D12CommandList::BindGraphicsPipeline(D3D12GraphicsPipeline* pso)
	{
		const auto& state = pso->GetDesc();

		m_commandList->SetPipelineState(pso->PipelineState());
		m_commandList->SetGraphicsRootSignature(pso->RootSignature());

		m_commandList->IASetPrimitiveTopology(D3D12Primitive(state.primitiveType));

		if (state.depthStencilState.stencilEnable)
		{
			m_commandList->OMSetStencilRef(state.depthStencilState.stencilRefValue);
		}

		if (pso->RequiresBlendFactor())
		{
			m_commandList->OMSetBlendFactor(&state.blendState.blendFactor.R);
		}
	}

	void D3D12CommandList::BindFramebuffer(D3D12GraphicsPipeline* pso, D3D12Framebuffer* fb)
	{
		const auto& state = pso->GetDesc();

		for (const auto& attachment : fb->GetDesc().colorAttachments)
		{
			if(attachment.texture)
				Transition(attachment.texture, (TRANSITION_STATES_FLAGS)TRANSITION_STATES::COMMON, (TRANSITION_STATES_FLAGS)TRANSITION_STATES::RENDER_TARGET);
		}

		if (fb->GetDesc().depthAttachment.texture)
		{
			const auto& attachment = fb->GetDesc().depthAttachment.texture;

			TRANSITION_STATES resourceState = TRANSITION_STATES::DEPTH_READ;
			if (state.depthStencilState.depthWriteEnable == true || state.depthStencilState.stencilWriteMask != 0)
				resourceState = TRANSITION_STATES::DEPTH_WRITE;

			Transition(attachment, (TRANSITION_STATES_FLAGS)TRANSITION_STATES::COMMON, (TRANSITION_STATES_FLAGS)resourceState);
		}

		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, MAX_RENDER_TARGETS> RTVs;
		int renderTargetCount = 0;
		for (int i = 0; i < fb->GetDesc().colorAttachments.size(); ++i)
		{
			if (fb->GetDesc().colorAttachments[i].texture)
			{
				renderTargetCount++;
				D3D12TextureView* view = (D3D12TextureView*)fb->GetDesc().colorAttachments[i].texture->GetView(ITextureView::ViewType::RENDER_TARGET);
				RTVs[i] = (view->CPU);
			}
			else
				RTVs[i] = D3D12_CPU_DESCRIPTOR_HANDLE(0);
			
		}

		D3D12_CPU_DESCRIPTOR_HANDLE DSV = {};
		if (fb->GetDesc().depthAttachment.texture)
		{
			D3D12TextureView* view = (D3D12TextureView*)fb->GetDesc().depthAttachment.texture->GetView(ITextureView::ViewType::DEPTH);
			DSV = view->CPU;
		}

		m_commandList->OMSetRenderTargets(renderTargetCount, RTVs.data(), false, fb->GetDesc().depthAttachment.texture ? &DSV : nullptr);
	}

	void D3D12CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		m_commandList->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
	}


}