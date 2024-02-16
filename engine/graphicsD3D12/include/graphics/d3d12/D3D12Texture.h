#pragma once
#include "graphics/interface/Texture.h"
#include <wrl/client.h>
#include "directx/d3d12.h"
#include "graphics/d3d12/D3D12Device.h"

namespace INF::GFX
{
	class D3D12TextureView : public ITextureView
	{
	public:
		DescriptorIndex descriptorIndex{ DescriptorIndexInvalid };
		D3D12_CPU_DESCRIPTOR_HANDLE CPU;
		D3D12_GPU_DESCRIPTOR_HANDLE GPU;
	};

	class D3D12Texture : public ITexture
	{
	public:
		D3D12Texture(D3D12Device* ownerDevice, const TextureDesc& desc);
		~D3D12Texture();
		const TextureDesc& GetDesc() const override;

		static TextureHandle CreateTexture(D3D12Device* ownerDevice, const TextureDesc& desc);
		static TextureHandle CreateTextureFromResource(D3D12Device* ownerDevice, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const TextureDesc& desc);
		ID3D12Resource* Resource() const { return m_resource.Get(); }

		const ITextureView* GetView(ITextureView::ViewType type) override;
	private:
		D3D12Device* m_ownerDevice;
		TextureDesc m_desc;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;

		D3D12TextureView m_rtv;
		D3D12TextureView m_srv;
	};

	class D3D12Sampler : public ISampler
	{
	public:
		D3D12Sampler(D3D12Device* ownerDevice, const SamplerDesc& desc);
		~D3D12Sampler();

		const SamplerDesc& GetDesc() const override;
		D3D12_GPU_DESCRIPTOR_HANDLE GPU() const { return m_GPU; }
	private:
		D3D12Device* m_device;
		SamplerDesc m_desc;

		DescriptorIndex m_descriptorIndex{ DescriptorIndexInvalid };
		D3D12_CPU_DESCRIPTOR_HANDLE m_CPU;
		D3D12_GPU_DESCRIPTOR_HANDLE m_GPU;
	};

	//D3D12 doesn't have the concept of a framebuffer but we still use it to group the target in FrameBufferDesc
	class D3D12Framebuffer : public IFramebuffer
	{
	public:
		D3D12Framebuffer(const FramebufferDesc& desc);
		virtual const FramebufferDesc& GetDesc() const override;
		const FramebufferInfo& GetInfo() const override;

	private:
		FramebufferDesc m_desc;
		FramebufferInfo m_info;
	};
}