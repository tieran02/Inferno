#include "infPCH.h"
#include "graphics/d3d12/D3D12Texture.h"
#include "graphics/interface/device.h"
#include "graphics/d3d12/D3D12Defines.h"


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
	if (m_srv.descriptorIndex != DescriptorIndexInvalid)
	{
		m_ownerDevice->SRVDescriptoHeap().ReleaseDescriptor(m_srv.descriptorIndex);
	}
}

D3D12_RESOURCE_DESC GetResourceDesc(const TextureDesc& desc)
{
	D3D12_RESOURCE_DESC resourceDesc{};

	resourceDesc.Width = desc.width;
	resourceDesc.Height = desc.height;
	resourceDesc.MipLevels = UINT16(desc.mipLevels);
	resourceDesc.Format = D3D12Format(desc.format);
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	switch (desc.dimension)
	{
	case TextureDimension::Texture2D:
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.DepthOrArraySize = UINT16(desc.arraySize);
		break;
	default:
		INF_ASSERT(false, "dimension not supported");
		break;
	}

	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	if(IsDepthFormat(desc.format))
		resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	return resourceDesc;
}

TextureHandle D3D12Texture::CreateTexture(D3D12Device* ownerDevice, const TextureDesc& desc)
{
	D3D12Texture* texture = new D3D12Texture(ownerDevice, desc);
	TextureHandle handle = TextureHandle(texture);
	D3D12_RESOURCE_DESC resourceDesc = GetResourceDesc(desc);
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	bool supportsOptimizedClear = (desc.initialState & (TRANSITION_STATES_FLAGS)TRANSITION_STATES::RENDER_TARGET);
	supportsOptimizedClear |= (desc.initialState & (TRANSITION_STATES_FLAGS)TRANSITION_STATES::DEPTH_READ);
	supportsOptimizedClear |= (desc.initialState & (TRANSITION_STATES_FLAGS)TRANSITION_STATES::DEPTH_WRITE);

	D3D12_CLEAR_VALUE optimizedClearValue = {};
	optimizedClearValue.Format = D3D12Format(desc.format);
	if (IsDepthFormat(desc.format)) 
	{
		optimizedClearValue.DepthStencil.Depth = 1.0f;
		optimizedClearValue.DepthStencil.Stencil = 0;
	}
	else
	{
		optimizedClearValue.Color[0] = 0.0f;
		optimizedClearValue.Color[1] = 0.0f;
		optimizedClearValue.Color[2] = 0.0f;
		optimizedClearValue.Color[3] = 0.0f;
	}

	VerifySuccess(ownerDevice->Device()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12TransitionFlags(desc.initialState),
		supportsOptimizedClear ? &optimizedClearValue : nullptr,
		IID_PPV_ARGS(&texture->m_resource)));

	texture->m_resource->SetName(desc.name);

	return handle;
}


TextureHandle D3D12Texture::CreateTextureFromResource(D3D12Device* ownerDevice, const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, const TextureDesc& desc)
{
	TextureHandle handle = TextureHandle(new D3D12Texture(ownerDevice,desc));
	static_cast<D3D12Texture*>(handle.get())->m_resource = resource;

	resource->SetName(desc.name);

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
		if (m_srv.descriptorIndex == DescriptorIndexInvalid)
		{
			m_srv.descriptorIndex = m_ownerDevice->SRVDescriptoHeap().AllocateDescriptor();
			m_srv.CPU = m_ownerDevice->SRVDescriptoHeap().GetCPUHandle(m_srv.descriptorIndex);
			m_srv.GPU = m_ownerDevice->SRVDescriptoHeap().GetGPUHandle(m_srv.descriptorIndex);

			m_ownerDevice->CreateShaderResourceView(m_srv.descriptorIndex, this);

			m_srv.format = m_desc.format;
			m_srv.type = ITextureView::ViewType::SHADER_RESOURCE;
		}
		return &m_srv;
	case INF::GFX::ITextureView::ViewType::DEPTH:
		if (m_dsv.descriptorIndex == DescriptorIndexInvalid)
		{
			m_dsv.descriptorIndex = m_ownerDevice->DSVDescriptoHeap().AllocateDescriptor();
			m_dsv.CPU = m_ownerDevice->DSVDescriptoHeap().GetCPUHandle(m_dsv.descriptorIndex);
			m_dsv.GPU = m_ownerDevice->DSVDescriptoHeap().GetGPUHandle(m_dsv.descriptorIndex);

			m_ownerDevice->CreateDepthhaderView(m_dsv.descriptorIndex, this);

			m_dsv.format = m_desc.format;
			m_dsv.type = ITextureView::ViewType::DEPTH;
		}
		return &m_dsv;
	default:
		break;
	}

	INF_ASSERT(false, "Failed to get texture view");
	return nullptr;
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
