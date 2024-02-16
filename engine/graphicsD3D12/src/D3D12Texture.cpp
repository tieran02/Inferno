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

const FramebufferDesc& D3D12Framebuffer::GetDesc() const
{
	return m_desc;
}

const FramebufferInfo& D3D12Framebuffer::GetInfo() const
{
	return m_info;
}

D3D12Sampler::D3D12Sampler(D3D12Device* ownerDevice, const SamplerDesc& samplerDesc) : m_device(ownerDevice), m_desc(samplerDesc)
{
	D3D12_SAMPLER_DESC desc;

	UINT reductionType;
	switch (samplerDesc.reductionType)
	{
	case ReductionType::COMPARISON:
		reductionType = D3D12_FILTER_REDUCTION_TYPE_COMPARISON;
		break;
	case ReductionType::MINIMUM:
		reductionType = D3D12_FILTER_REDUCTION_TYPE_MINIMUM;
		break;
	case ReductionType::MAXIMUM:
		reductionType = D3D12_FILTER_REDUCTION_TYPE_MAXIMUM;
		break;
	default:
		reductionType = D3D12_FILTER_REDUCTION_TYPE_STANDARD;
		break;
	}

	if (samplerDesc.anisotropy > 1.0f)
	{
		desc.Filter = D3D12_ENCODE_ANISOTROPIC_FILTER(reductionType);
	}
	else
	{
		desc.Filter = D3D12_ENCODE_BASIC_FILTER(
			samplerDesc.minFilter ? D3D12_FILTER_TYPE_LINEAR : D3D12_FILTER_TYPE_POINT,
			samplerDesc.magFilter ? D3D12_FILTER_TYPE_LINEAR : D3D12_FILTER_TYPE_POINT,
			samplerDesc.mipFilter ? D3D12_FILTER_TYPE_LINEAR : D3D12_FILTER_TYPE_POINT,
			reductionType);
	}

	D3D12_TEXTURE_ADDRESS_MODE* dest[] = { &desc.AddressU, &desc.AddressV, &desc.AddressW };
	for (int i = 0; i < 3; i++)
	{
		switch (samplerDesc.wrapMode[i])
		{
		case WrapMode::BORDER:
			*dest[i] = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			break;
		case WrapMode::CLAMP:
			*dest[i] = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			break;
		case WrapMode::WRAP:
			*dest[i] = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			break;
		}
	}

	desc.MipLODBias = samplerDesc.mipBias;
	desc.MaxAnisotropy = max((uint32_t)samplerDesc.anisotropy, 1U);
	desc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
	desc.BorderColor[0] = samplerDesc.borderColor.R;
	desc.BorderColor[1] = samplerDesc.borderColor.G;
	desc.BorderColor[2] = samplerDesc.borderColor.B;
	desc.BorderColor[3] = samplerDesc.borderColor.A;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D12_FLOAT32_MAX;

	m_descriptorIndex = m_device->SamplerDescriptoHeap().AllocateDescriptor();
	m_CPU = m_device->SamplerDescriptoHeap().GetCPUHandle(m_descriptorIndex);
	m_GPU = m_device->SamplerDescriptoHeap().GetGPUHandle(m_descriptorIndex);

	m_device->Device()->CreateSampler(&desc, m_CPU);
}

D3D12Sampler::~D3D12Sampler()
{
	m_device->SamplerDescriptoHeap().ReleaseDescriptor(m_descriptorIndex);
}

const SamplerDesc& D3D12Sampler::GetDesc() const
{
	return m_desc;
}
