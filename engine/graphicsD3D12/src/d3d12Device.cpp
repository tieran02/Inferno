#pragma once
#include "infPCH.h"
#include "graphics/d3d12/d3d12Device.h"
#include "graphics/d3d12/D3D12Defines.h"
#include "graphics/d3d12/D3D12Shader.h"
#include <wrl/client.h>
#include <directx/d3d12.h>
#include <dxgi1_4.h>
#include "graphics/d3d12/D3D12Texture.h"
#include "graphics/d3d12/D3D12Pipeline.h"
#include "graphics/d3d12/D3D12Buffer.h"
#include "graphics/d3d12/D3D12Descriptor.h"

namespace INF::GFX
{

	D3D12DescriptorHeap::D3D12DescriptorHeap() : m_heapType(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
	{

	}

	bool D3D12DescriptorHeap::CreateResources(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t descriptorCount, bool shaderVisible)
	{
		m_heap = nullptr;

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = heapType;
		heapDesc.NumDescriptors = descriptorCount;
		heapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		VerifySuccess(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap)));

		m_descriptors.resize(heapDesc.NumDescriptors);
		m_heapType = heapDesc.Type;

		const D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();
		const D3D12_GPU_DESCRIPTOR_HANDLE gpuStart = shaderVisible ? m_heap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE(0);
		const uint32_t descriptorSize = device->GetDescriptorHandleIncrementSize(heapType);

		for (int i = 0; i < m_descriptors.size(); ++i)
		{
			m_descriptors[i].Cpu = D3D12_CPU_DESCRIPTOR_HANDLE(cpuStart.ptr + (i * descriptorSize));

			if (shaderVisible)
				m_descriptors[i].Gpu = D3D12_GPU_DESCRIPTOR_HANDLE(gpuStart.ptr + (i * descriptorSize));
			else
				m_descriptors[i].Gpu = gpuStart;

			m_freeDescriptors.push_back(i);
		}

		return true;
	}

	DescriptorIndex D3D12DescriptorHeap::AllocateDescriptor()
	{
		INF_ASSERT(!m_freeDescriptors.empty(), "No free descriptors left to allocate, either increase descriptor count or release unused descriptos");
		DescriptorIndex index = m_freeDescriptors.front();
		m_freeDescriptors.pop_front();
		return index;
	}

	void D3D12DescriptorHeap::ReleaseDescriptor(DescriptorIndex index)
	{
		m_freeDescriptors.push_front(index);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetCPUHandle(DescriptorIndex index)
	{
		INF_ASSERT(index >= 0 && index < m_descriptors.size(), "Index out of rangr");
		return m_descriptors.at(index).Cpu;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetGPUHandle(DescriptorIndex index)
	{
		INF_ASSERT(index >= 0 && index < m_descriptors.size(), "Index out of rangr");
		return m_descriptors.at(index).Gpu;
	}

	D3D12Device::D3D12Device(const DeviceCreationParameters& createInfo)
	{
		if(createInfo.enableDebugValidation)
			CreateDebugController();

		CreateFactory();
		CreateAdapter();

		CreateGraphicsCommandAllocator();
		CreateDescriptorHeaps();

	}

	void D3D12Device::CreateDebugController()
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> dc;
		VerifySuccess(D3D12GetDebugInterface(IID_PPV_ARGS(&dc)));

		VerifySuccess(dc->QueryInterface(IID_PPV_ARGS(&m_debugController)));

		m_debugController->EnableDebugLayer();
		m_debugController->SetEnableGPUBasedValidation(true);
	}

	void D3D12Device::CreateFactory()
	{
		UINT dxgiFactoryFlags = 0;
		if (m_debugController)
		{
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}

		VerifySuccess(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));
	}

	void D3D12Device::CreateAdapter()
	{
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_dxgiFactory->EnumAdapters1(adapterIndex, &m_adapter); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			HRESULT hr = m_adapter->GetDesc1(&desc);
			if (FAILED(hr))
				continue;

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				continue;
			}

			// Check to see if the adapter supports Direct3D 12,
			// but don't create the actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
		INF_ASSERT(m_adapter.Get(), "Failed to create adapter"); //If assert is hit then there was now supported adapteru

		//Now create the device here
		VerifySuccess(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

		if (m_debugController)
		{
			VerifySuccess(m_device->QueryInterface(IID_PPV_ARGS(&m_debugDevice)));
		}
	}

	void D3D12Device::CreateGraphicsCommandAllocator()
	{
		m_graphicsQueue = D3D12QueueHandle(new D3D12Queue(m_device.Get(), CommandQueue::GRAPHICS));

		VerifySuccess(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_graphicsCommandAllocator)));
	}

	CommandListeHandle D3D12Device::CreateCommandList(CommandQueue queueType)
	{
		return CommandListeHandle(new D3D12CommandList(this, m_graphicsCommandAllocator, queueType));
	}

	uint64_t D3D12Device::ExecuteCommandLists(const ICommandList* commandLists, uint32_t commandListCount)
	{
		const D3D12CommandList* d3d12CommandLists = static_cast<const D3D12CommandList*>(commandLists);
		return m_graphicsQueue->ExecuteCommandLists(d3d12CommandLists->D3D(), commandListCount);
	}

	void D3D12Device::ImmediateSubmit(ImmediateSubmitFn ImmediateFn)
	{
		CommandListeHandle cmd = CreateCommandList(CommandQueue::GRAPHICS);
		cmd->Open();
		ImmediateFn(cmd.get());
		cmd->Close();
		ExecuteCommandLists(cmd.get(), 1);
		WaitForIdle();
	}

	void D3D12Device::WaitForIdle()
	{
		//Wait for the graphics queue to finish, if we have more queues later we will need to add them (E.G compute/copy)
		m_graphicsQueue->Wait();
	}

	ShaderHandle D3D12Device::CreateShader(const ShaderDesc& desc)
	{
		return ShaderHandle(new D3D12Shader(desc));
	}

	FramebufferHandle D3D12Device::CreateFramebuffer(const FramebufferDesc& desc)
	{
		return FramebufferHandle(new D3D12Framebuffer(desc));
	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> D3D12Device::CreatePipelineState(const GraphicsPipelineDesc& state, IFramebuffer* fb, ID3D12RootSignature* rootSignature)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};

		desc.VS = static_cast<D3D12Shader*>(state.VS.get())->D3D();
		desc.PS = static_cast<D3D12Shader*>(state.PS.get())->D3D();

		desc.BlendState = D3D12BlendState(state.blendState);

		desc.DepthStencilState = D3D12DepthStencilState(state.depthStencilState);

		desc.RasterizerState = D3D12RasterizerState(state.rasterState);

		desc.PrimitiveTopologyType = D3D12PrimitiveType(state.primitiveType);

		desc.DSVFormat = D3D12Format(fb->GetInfo().depthFormat);

		//dont support MSAA
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		desc.NumRenderTargets = MAX_RENDER_TARGETS;
		for (int i = 0; i < fb->GetInfo().colorFormats.size(); i++)
		{
			desc.RTVFormats[i] = D3D12Format(fb->GetInfo().colorFormats[i]);
		}

		//Create the input layout
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputStates(state.inputLayoutDesc.size());
		desc.InputLayout.NumElements = (UINT)inputStates.size();
		for (int i = 0; i < state.inputLayoutDesc.size(); ++i)
		{
			inputStates[i].SemanticName = state.inputLayoutDesc[i].semanticName.c_str();
			inputStates[i].SemanticIndex = 0;
			inputStates[i].InputSlot = 0;
			inputStates[i].Format = D3D12Format(state.inputLayoutDesc[i].format);
			inputStates[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		}
		desc.InputLayout.pInputElementDescs = inputStates.data();
		desc.SampleMask = ~0u;

		desc.pRootSignature = rootSignature;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
		VerifySuccess(m_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState)));

		return pipelineState;
	}
	
	GraphicsPipelineHandle D3D12Device::CreateGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb)
	{
		//Get root sig from descriptor layout handle
		D3D12DescriptorLayout* layout = static_cast<D3D12DescriptorLayout*>(desc.descriptorLayout.get());

		auto pso = CreatePipelineState(desc, fb, layout->RootSignature());
		return GraphicsPipelineHandle(new D3D12GraphicsPipeline(desc, pso, layout->RootSignature(), fb));
	}

	DescriptorLayoutHandle D3D12Device::CreateDescriptorLayout(const DescriptorLayoutDesc desc)
	{
		return DescriptorLayoutHandle(new D3D12DescriptorLayout(m_device.Get(), desc));
	}

	DescriptorSetHandle D3D12Device::CreateDescriptorSet(const DescriptorSetDesc& desc, IDescriptorLayout* layout)
	{
		return DescriptorSetHandle(new D3D12DescriptorSet(layout, desc));
	}

	BufferHandle D3D12Device::CreateBuffer(const BufferDesc& desc)
	{
		return BufferHandle(new D3D12Buffer(this, desc));
	}

	VertexBufferHandle D3D12Device::CreateVertexBuffer(const VertexBufferDesc& desc)
	{
		return VertexBufferHandle(new D3D12VertexBuffer(this, desc));
	}

	IndexBufferHandle D3D12Device::CreateIndexBuffer(const IndexBufferDesc& desc)
	{
		return IndexBufferHandle(new D3D12IndexBuffer(this, desc));
	}

	void* D3D12Device::MapBuffer(IBuffer* buffer, uint32_t readStart /*= 0*/, uint32_t readEnd /*= 0*/)
	{
		INF_ASSERT(buffer->GetDesc().access != CpuVisible::NONE, "Buffer not CPU visable, can't map")
		if (buffer->GetDesc().access == CpuVisible::NONE)
			return nullptr;

		D3D12Buffer* d3d12Buffer = static_cast<D3D12Buffer*>(buffer);
		D3D12_RANGE readRange(readStart, readEnd);
		void* data;
		VerifySuccess(d3d12Buffer->Resource()->Map(0, &readRange, &data));
		return data;
	}

	void D3D12Device::UnmapBuffer(IBuffer* buffer, uint32_t writeStart /*= 0*/, uint32_t writeEnd /*= 0*/)
	{
		INF_ASSERT(buffer->GetDesc().access != CpuVisible::NONE, "Buffer not CPU visable, can't unmap")
			if (buffer->GetDesc().access == CpuVisible::NONE)
				return;

		D3D12_RANGE writeRange(writeStart, writeEnd);
		D3D12Buffer* d3d12Buffer = static_cast<D3D12Buffer*>(buffer);
		d3d12Buffer->Resource()->Unmap(0, &writeRange);
	}

	SamplerHandle D3D12Device::CreateSampler(const SamplerDesc& desc)
	{
		return SamplerHandle(new D3D12Sampler(this, desc));
	}

	TextureHandle D3D12Device::CreateTexture(const TextureDesc& desc)
	{
		return D3D12Texture::CreateTexture(this, desc);
	}

	void D3D12Device::CreateDescriptorHeaps()
	{
		m_SRVDescriptorHeap.CreateResources(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024, true);
		m_RTVDescriptorHeap.CreateResources(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1024, false);
		m_DSVDescriptorHeap.CreateResources(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1024, false);
		m_SamplerDescriptorHeap.CreateResources(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 100,true);
	}

	void D3D12Device::CreateRenderTargetView(DescriptorIndex descriptorIndex, ITexture* texture)
	{
		D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
		const TextureDesc& textureDesc = texture->GetDesc();

		viewDesc.Format = D3D12Format(textureDesc.format);
		switch (textureDesc.dimension)
		{
		case TextureDimension::Texture2D:
			viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			viewDesc.Texture2D.MipSlice = 0; //todo mip subresources
			break;
		default:
			INF_ASSERT(false, "Dimension not supported");
			break;
		}
		m_device->CreateRenderTargetView(static_cast<D3D12Texture*>(texture)->Resource(), &viewDesc, m_RTVDescriptorHeap.GetCPUHandle(descriptorIndex));
	}

	void D3D12Device::CreateShaderResourceView(DescriptorIndex descriptorIndex, ITexture* texture)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
		const TextureDesc& textureDesc = texture->GetDesc();

		viewDesc.Format = D3D12Format(textureDesc.format);
		viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		viewDesc.Texture2D.MipLevels = textureDesc.mipLevels;
		switch (textureDesc.dimension)
		{
		case TextureDimension::Texture2D:
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			break;
		default:
			INF_ASSERT(false, "Dimension not supported");
			break;
		}

		m_device->CreateShaderResourceView(static_cast<D3D12Texture*>(texture)->Resource(), &viewDesc, m_SRVDescriptorHeap.GetCPUHandle(descriptorIndex));
	}

}