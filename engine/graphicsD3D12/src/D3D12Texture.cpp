#include "infPCH.h"
#include "graphics/d3d12/D3D12Texture.h"
#include "graphics/interface/device.h"

using namespace INF::GFX;


D3D12Texture::D3D12Texture(D3D12Device* ownerDevice, const TextureDesc& desc) : m_ownerDevice(ownerDevice), m_desc(desc)
{

}

D3D12Texture::~D3D12Texture()
{
	if (m_rtv.descriptorIndex != DescriptorIndexInvalid)
	{
		m_ownerDevice->RTVDescriptoHeap().ReleaseDescriptor(m_rtv.descriptorIndex);
	}
}


TextureHandle D3D12Texture::CreateTextureFromResource(D3D12Device* ownerDevice, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const TextureDesc& desc)
{
	TextureHandle handle = TextureHandle(new D3D12Texture(ownerDevice,desc));
	static_cast<D3D12Texture*>(handle.get())->m_resource = resource;

	return handle;
}

const TextureDesc& D3D12Texture::GetDesc() const
{
	return m_desc;
}

const ITextureView* D3D12Texture::GetView(ITextureView::ViewType type)
{
	switch (type)
	{
	case INF::GFX::ITextureView::ViewType::RENDER_TARGET:
		if (m_rtv.descriptorIndex == DescriptorIndexInvalid)
		{
			m_rtv.descriptorIndex = m_ownerDevice->RTVDescriptoHeap().AllocateDescriptor();
			m_rtv.CPU = m_ownerDevice->RTVDescriptoHeap().GetCPUHandle(m_rtv.descriptorIndex);
			m_rtv.GPU = m_ownerDevice->RTVDescriptoHeap().GetGPUHandle(m_rtv.descriptorIndex);

			m_ownerDevice->CreateRenderTargetView(m_rtv.descriptorIndex, this);

			m_rtv.format = m_desc.format;
			m_rtv.type = ITextureView::ViewType::RENDER_TARGET;
		}

		return &m_rtv;
		break;
	case INF::GFX::ITextureView::ViewType::SHADER_RESOURCE:
		break;
	case INF::GFX::ITextureView::ViewType::DEPTH:
		break;
	default:
		break;
	}
}

D3D12Framebuffer::D3D12Framebuffer(const FramebufferDesc& desc) : m_desc(desc), m_info(desc)
{

}

const INF::GFX::FramebufferDesc& D3D12Framebuffer::GetDesc() const
{
	return m_desc;
}

const INF::GFX::FramebufferInfo& D3D12Framebuffer::GetInfo() const
{
	return m_info;
}
