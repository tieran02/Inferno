#include "infPCH.h"
#include "graphics/d3d12/D3D12CommandList.h"
#include "graphics/d3d12/D3D12Defines.h"
#include "graphics/interface/Texture.h"
#include "graphics/d3d12/D3D12Texture.h"
#include "graphics/d3d12/D3D12Pipeline.h"
#include "graphics/d3d12/D3D12Buffer.h"
#include "graphics/d3d12/D3D12Descriptor.h"
#include "graphics/d3d12/D3D12Device.h"
#include "graphics/Bitmap.h"
#include "graphics/View.h"

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

	uint64_t D3D12Queue::ExecuteCommandList(D3D12CommandList* commandList)
	{
		ID3D12CommandList* ppCommandLists[] = { static_cast<ID3D12CommandList*>(commandList->D3D()) };
		Queue->ExecuteCommandLists(1, ppCommandLists);
		LastSubmittedInstance++;
		Queue->Signal(Fence.Get(), LastSubmittedInstance);

		m_commandListsInFlight.push(std::make_pair(commandList, LastSubmittedInstance));
		return LastSubmittedInstance;
	}

	void D3D12Queue::Wait()
	{
		ReleaseInFlight(true);
	}

	uint64_t D3D12Queue::UpdateLastCompletedInstance()
	{
		if (LastCompletedInstance < LastSubmittedInstance)
		{
			LastCompletedInstance = Fence->GetCompletedValue();
		}
		return LastCompletedInstance;
	}

	void D3D12Queue::EndFrame()
	{
		ReleaseInFlight(false);
	}

	void D3D12Queue::ReleaseInFlight(bool wait)
	{
		uint64_t lastcomplete = UpdateLastCompletedInstance();

		if (wait)
		{
			if (lastcomplete < LastSubmittedInstance)
			{
				// Test if the fence has been reached
				if (Fence->GetCompletedValue() < LastSubmittedInstance)
				{
					// If it's not, wait for it to finish using an event
					ResetEvent(m_fenceEvent);
					Fence->SetEventOnCompletion(LastSubmittedInstance, m_fenceEvent);
					WaitForSingleObject(m_fenceEvent, INFINITE);
					lastcomplete = UpdateLastCompletedInstance();
				}
			}
		}

		while (!m_commandListsInFlight.empty())
		{
			const auto& pair = m_commandListsInFlight.front();
			if (lastcomplete >= pair.second)
			{
				//in flight frame has finised, give it back to the command list
				for (FrameCommandList& cmd : pair.first->m_commandLists)
				{
					ID3D12GraphicsCommandList* current = cmd.CommandList.Get();
					ID3D12GraphicsCommandList* inflight = pair.first->D3D();
					if (current == inflight)
					{
						pair.first->m_freeCommandLists.push(&cmd);
						break;
					}
				}
				m_commandListsInFlight.pop();

			}
			else
			{
				break;
			}
		}
	}

	D3D12CommandList::D3D12CommandList(D3D12Device* d3dDevice, CommandQueue queueType) : m_queueType(queueType)
	{
		m_device = d3dDevice;
	}


	D3D12CommandList::~D3D12CommandList()
	{
		m_commandList = nullptr;
		m_freeCommandLists = {};
		m_commandLists.clear();
	}


	void D3D12CommandList::Open()
	{
		FrameCommandList* commandList;
		if (m_freeCommandLists.empty())
		{
			FrameCommandList frameCommandList;
			VerifySuccess(m_device->Device()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameCommandList.CommandAllocator)));

			switch (m_queueType)
			{
			case INF::GFX::CommandQueue::GRAPHICS:
				VerifySuccess(m_device->Device()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameCommandList.CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&frameCommandList.CommandList)));
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
			frameCommandList.CommandList->Close();

			m_commandLists.push_back(frameCommandList);
			commandList = &m_commandLists.back();
			commandList->CommandList->SetName(L"Test");
		}
		else
		{
			commandList = m_freeCommandLists.front();
			m_freeCommandLists.pop();
		}

		commandList->CommandAllocator->Reset();

		commandList->CommandList->Reset(commandList->CommandAllocator.Get(), nullptr);
		m_commandList = commandList->CommandList.Get();

		m_referencedBuffers.clear();
	}

	void D3D12CommandList::Close()
	{
		//transition all resources to initial state 
		for (const auto& state : m_textureStates)
		{
			TRANSITION_STATES_FLAGS initialState = state.first->GetDesc().initialState;
			if(initialState != state.second)
				TransitionResource(static_cast<D3D12Texture*>(state.first)->Resource(), state.second, initialState);
		}
		m_textureStates.clear();

		for (const auto& state : m_bufferStates)
		{
			TRANSITION_STATES_FLAGS initialState = state.first->GetDesc().initialState;
			if (initialState != state.second)
				TransitionResource(static_cast<D3D12Buffer*>(state.first)->Resource(), state.second, initialState);
		}
		m_bufferStates.clear();

		m_commandList->Close();
	}

	void D3D12CommandList::TransitionResource(ID3D12Resource* resource, TRANSITION_STATES_FLAGS from, TRANSITION_STATES_FLAGS to)
	{
		D3D12_RESOURCE_BARRIER renderTargetBarrier;
		renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

		renderTargetBarrier.Transition.pResource = resource;
		renderTargetBarrier.Transition.StateBefore = D3D12TransitionFlags(from);
		renderTargetBarrier.Transition.StateAfter = D3D12TransitionFlags(to);
		renderTargetBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		m_commandList->ResourceBarrier(1, &renderTargetBarrier);
	}

	void D3D12CommandList::Transition(ITexture* texture, TRANSITION_STATES_FLAGS to)
	{
		//Get from state
		auto from = GetTextureState(texture);
		if (from == to)
			return;

		TransitionResource(static_cast<D3D12Texture*>(texture)->Resource(), from, to);
		m_textureStates[texture] = to;
	}

	void D3D12CommandList::Transition(IBuffer* buffer, TRANSITION_STATES_FLAGS to)
	{
		//Get from state
		auto from = GetBufferState(buffer);
		if (from == to)
			return;

		TransitionResource(static_cast<D3D12Buffer*>(buffer)->Resource(), from, to);
		m_bufferStates[buffer] = to;
	}

	void D3D12CommandList::ClearColor(ITexture* texture, const Color& color)
	{
		Transition(texture, (TRANSITION_STATES_FLAGS)TRANSITION_STATES::RENDER_TARGET);

		const D3D12TextureView* textureView = (const D3D12TextureView*)texture->GetView(ITextureView::ViewType::RENDER_TARGET);
		INF_ASSERT(textureView, "Invalid Texture view");
		m_commandList->ClearRenderTargetView(textureView->CPU, &color.R, 0, nullptr);
	}

	void D3D12CommandList::SetGraphicsState(const GraphicsState& state)
	{
		if (state.view) 
		{
			SetViewport(state.view->GetViewport());
			SetScissor(state.view->GetScissor());
		}

		D3D12GraphicsPipeline* pso = static_cast<D3D12GraphicsPipeline*>(state.pipeline);
		D3D12Framebuffer* framebuffer = static_cast<D3D12Framebuffer*>(state.framebuffer);
		D3D12DescriptorSet* descriptorSet = static_cast<D3D12DescriptorSet*>(state.descriptorSet);

		if(pso)
			BindGraphicsPipeline(pso);
		if (pso && framebuffer)
			BindFramebuffer(pso, framebuffer);

		if(descriptorSet)
			BindDescriptorSet(descriptorSet);

		D3D12VertexBuffer* vertexBuffer = static_cast<D3D12VertexBuffer*>(state.vertexBuffer);
		if(vertexBuffer)
			m_commandList->IASetVertexBuffers(0,1, &vertexBuffer->View());

		D3D12IndexBuffer* indexBuffer = static_cast<D3D12IndexBuffer*>(state.indexBuffer);
		if (indexBuffer)
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
			if (attachment.texture)
				Transition(attachment.texture, (TRANSITION_STATES_FLAGS)TRANSITION_STATES::RENDER_TARGET);
		}

		if (fb->GetDesc().depthAttachment.texture)
		{
			const auto& attachment = fb->GetDesc().depthAttachment.texture;

			TRANSITION_STATES resourceState = TRANSITION_STATES::DEPTH_READ;
			if (state.depthStencilState.depthWriteEnable == true || state.depthStencilState.stencilWriteMask != 0)
				resourceState = TRANSITION_STATES::DEPTH_WRITE;

			Transition(attachment, (TRANSITION_STATES_FLAGS)resourceState);
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

	void D3D12CommandList::BindDescriptorSet(D3D12DescriptorSet* set)
	{
		//TODO we should only set the heaps if they changed
		ID3D12DescriptorHeap* heaps[2] = { m_device->SRVDescriptoHeap().Heap(), m_device->SamplerDescriptoHeap().Heap()};
		m_commandList->SetDescriptorHeaps(2, heaps);

		std::array<std::pair<ShaderType, const StageDescriptorSetDesc*>, 3> stages
		{
			std::make_pair(ShaderType::Vertex, &set->GetDesc().VS),
			std::make_pair(ShaderType::Pixel, &set->GetDesc().PS),
			std::make_pair(ShaderType::All, &set->GetDesc().ALL),
		};


		D3D12DescriptorLayout* layout = static_cast<D3D12DescriptorLayout*>(set->GetLayout());
		for (const auto& stage : stages)
		{
			for (const DescriptorSetItem& setItem : *stage.second)
			{
				if(setItem.slot == UINT_MAX)
					continue;

				uint32_t rootIndex = layout->GetRootParamIndex(stage.first,setItem);

				switch (setItem.type)
				{
				case ResourceType::TEXTURE_SRV:
				{
					D3D12Texture* texture = static_cast<D3D12Texture*>(setItem.resourceHandle.texture);
					const D3D12TextureView* view = static_cast<const D3D12TextureView*>(texture->GetView(ITextureView::ViewType::SHADER_RESOURCE));

					m_commandList->SetGraphicsRootDescriptorTable(rootIndex, view->GPU);
					break;
				}
				case ResourceType::BUFFER_SRV:
					throw std::logic_error("The method or operation is not implemented.");
					break;
				case ResourceType::CONSTANTBUFFER:
				{
					D3D12Buffer* buffer = static_cast<D3D12Buffer*>(setItem.resourceHandle.buffer);
					D3D12BufferView* view = static_cast<D3D12BufferView*>(buffer->GetView());

					m_commandList->SetGraphicsRootDescriptorTable(rootIndex, view->GPU);
					break;
				}
				case ResourceType::SAMPLER:
				{
					D3D12Sampler* sampler = static_cast<D3D12Sampler*>(setItem.resourceHandle.sampler);
					m_commandList->SetGraphicsRootDescriptorTable(rootIndex, sampler->GPU());
					break;
				}
				default:
					INF_ASSERT(false, "set item type not supported")
					break;
				}
			}
		}
	}


	void D3D12CommandList::CopyBuffer(IBuffer* dest, uint32_t destOffset, IBuffer* src, uint32_t srcOffset, size_t size)
	{
		D3D12Buffer* d3d12Dest = static_cast<D3D12Buffer*>(dest);
		D3D12Buffer* d3d12Src = static_cast<D3D12Buffer*>(src);

		GFX::TRANSITION_STATES_FLAGS destState = GetBufferState(d3d12Dest);
		GFX::TRANSITION_STATES_FLAGS srcState = GetBufferState(d3d12Src);

		INF_ASSERT(d3d12Dest->GetDesc().access == CpuVisible::NONE, "destination is CPU visible and can't be copied");
		if (d3d12Dest->GetDesc().access != CpuVisible::NONE)
			return;

		if (d3d12Src->GetDesc().access == CpuVisible::NONE)
			Transition(d3d12Src, (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::COPY_SOURCE);

		Transition(d3d12Dest, (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::COPY_DEST);

		m_commandList->CopyBufferRegion(d3d12Dest->Resource(), destOffset, d3d12Src->Resource(), srcOffset, size);

		if (d3d12Src->GetDesc().access == CpuVisible::NONE)
			Transition(d3d12Src, srcState);

		Transition(d3d12Dest, destState);
	}

	void D3D12CommandList::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		m_commandList->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
	}

	inline static uint32_t CalcSubresource(uint32_t MipSlice, uint32_t ArraySlice, uint32_t PlaneSlice, uint32_t MipLevels, uint32_t ArraySize)
	{
		return MipSlice + (ArraySlice * MipLevels) + (PlaneSlice * MipLevels * ArraySize);
	}

	void D3D12CommandList::WriteTexture(ITexture* dest, const Bitmap& bitmap)
	{
		D3D12Texture* destTexture = static_cast<D3D12Texture*>(dest);
		GFX::TRANSITION_STATES_FLAGS destState = GetTextureState(destTexture);

		Transition(destTexture, (GFX::TRANSITION_STATES_FLAGS)GFX::TRANSITION_STATES::COPY_DEST);
		const uint32_t uploadBufferSize = (static_cast<uint32_t>(bitmap.Size()) + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) & ~D3D12_TEXTURE_DATA_PITCH_ALIGNMENT; // size is required to be 256-byte aligned.

		BufferDesc uploadBufferDesc;
		uploadBufferDesc.access = CpuVisible::WRITE;
		uploadBufferDesc.byteSize = uploadBufferSize;
		uploadBufferDesc.name = "upload buffer";
		BufferHandle uploadBuffer = m_device->CreateBuffer(uploadBufferDesc);

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = bitmap.Data();
		textureData.RowPitch = bitmap.Width() * bitmap.ComponentCount();
		textureData.SlicePitch = textureData.RowPitch * bitmap.Height();

		//add the temp upload buffer to referenced buffer so it doesn't get deleted until next command buffer open or command buffer destroyed
		m_referencedBuffers.emplace_back(uploadBuffer);

		UpdateSubresources(m_commandList, destTexture->Resource(), static_cast<D3D12Buffer*>(uploadBuffer.get())->Resource(), 0, 0, 1, &textureData);

		Transition(destTexture, destState);
	}

	TRANSITION_STATES_FLAGS D3D12CommandList::GetTextureState(ITexture* texture)
	{
		auto it = m_textureStates.find(texture);
		
		if (it == m_textureStates.end())
		{
			//Texture state isn't tracked for this texture, create initial state for tracking
			it = m_textureStates.emplace(texture, texture->GetDesc().initialState).first;
		}

		return it->second;
	}

	TRANSITION_STATES_FLAGS D3D12CommandList::GetBufferState(IBuffer* buffer)
	{
		auto it = m_bufferStates.find(buffer);

		if (it == m_bufferStates.end())
		{
			//Texture state isn't tracked for this texture, create initial state for tracking
			it = m_bufferStates.emplace(buffer, buffer->GetDesc().initialState).first;
		}

		return it->second;
	}

}