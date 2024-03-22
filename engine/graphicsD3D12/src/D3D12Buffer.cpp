#include "infPCH.h"
#include "graphics/d3d12/D3D12Buffer.h"
#include "core/Assert.h"
#include "graphics/d3d12/D3D12Device.h"
#include "graphics/d3d12/D3D12Defines.h"
#include "directx/d3d12.h"

using namespace INF::GFX;

D3D12Buffer::D3D12Buffer(D3D12Device* device, const BufferDesc& desc) : m_desc(desc), m_device(device)
{
	constexpr uint32_t CONSTANT_BUFFER_ALINGMENT = 256;

	D3D12_RESOURCE_DESC bufferDesc = {};
	switch (desc.usage)
	{
	case BufferUsage::GENERIC:
	case BufferUsage::VERTEX:
	case BufferUsage::INDEX:
		m_desc.byteSize = desc.byteSize;
		break;
	case BufferUsage::CONSTANT:
		m_desc.byteSize = desc.byteSize + (CONSTANT_BUFFER_ALINGMENT - (desc.byteSize % CONSTANT_BUFFER_ALINGMENT)); //Constant buffers are 256 alligned
		break;
	default:
		INF_ASSERT(false, "Usage not supported");
		break;
	}

	//For D3D12 buffers with this will not create a d3d12 resource on creation, instead use commandList->WriteBuffer to allocate memory from a preallocated chunk.
	if (desc.onlyValidDuringCommandList)
		return;

	bufferDesc.Width = m_desc.byteSize;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.SampleDesc.Quality = 0;
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_HEAP_PROPERTIES heapProps = {};
	D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

	switch (desc.access)
	{
	case CpuVisible::NONE:
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		initialState = D3D12TransitionFlags(desc.initialState);
		break;
	case CpuVisible::READ:
		heapProps.Type = D3D12_HEAP_TYPE_READBACK;
		initialState = D3D12_RESOURCE_STATE_COPY_DEST;
		break;
	case CpuVisible::WRITE:
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
		break;
	default:
		INF_ASSERT(false, "Access not supported");
		break;
	}

	VerifySuccess(device->Device()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		initialState,
		nullptr,
		IID_PPV_ARGS(&m_resource)));


	if (m_desc.usage == BufferUsage::CONSTANT) 
	{
		m_view.descriptorIndex = device->SRVDescriptoHeap().AllocateDescriptor();
		m_view.CPU = device->SRVDescriptoHeap().GetCPUHandle(m_view.descriptorIndex);
		m_view.GPU = device->SRVDescriptoHeap().GetGPUHandle(m_view.descriptorIndex);

		D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = { m_resource->GetGPUVirtualAddress(), (UINT)bufferDesc.Width };
		m_device->Device()->CreateConstantBufferView(&viewDesc, m_view.CPU);
	}

	m_resource->SetName(desc.name.c_str());
}

BufferDesc D3D12Buffer::GetDesc()
{
	return m_desc;
}

IBufferView* D3D12Buffer::GetView()
{
	return &m_view;
}

D3D12Buffer::~D3D12Buffer()
{
	if(m_view.descriptorIndex != DescriptorIndexInvalid)
		m_device->SRVDescriptoHeap().ReleaseDescriptor(m_view.descriptorIndex);
}

void D3D12Buffer::CreateConstantBufferView(DescriptorIndex descriptorIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress)
{
	//INF_ASSERT(m_view.descriptorIndex == DescriptorIndexInvalid, "Buffer already has a view");
	m_view.descriptorIndex = descriptorIndex;
	m_view.CPU = m_device->SRVDescriptoHeap().GetCPUHandle(descriptorIndex);
	m_view.GPU = m_device->SRVDescriptoHeap().GetGPUHandle(descriptorIndex);

	D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = { gpuVirtualAddress, (UINT)m_desc.byteSize };
	m_device->Device()->CreateConstantBufferView(&viewDesc, m_view.CPU);
}

BufferDesc ConvertVertexDesc(const VertexBufferDesc& desc)
{
	BufferDesc bufferDesc;
	bufferDesc.access = desc.access;
	bufferDesc.byteSize = desc.byteSize;
	bufferDesc.name = desc.name;
	bufferDesc.usage = BufferUsage::VERTEX;
	bufferDesc.initialState = (TRANSITION_STATES_FLAGS)TRANSITION_STATES::VERTEX_BUFFER;
	return bufferDesc;
}

D3D12VertexBuffer::D3D12VertexBuffer(D3D12Device* device, VertexBufferDesc desc) : m_buffer(device, ConvertVertexDesc(desc)), m_desc(desc)
{
	m_view.BufferLocation = m_buffer.Resource()->GetGPUVirtualAddress();
	m_view.SizeInBytes = desc.byteSize;
	m_view.StrideInBytes = desc.strideInBytes;
}

VertexBufferDesc D3D12VertexBuffer::GetDesc()
{
	return m_desc;
}

IBuffer* D3D12VertexBuffer::GetBuffer()
{
	return &m_buffer;
}

BufferDesc ConvertIndexDesc(const IndexBufferDesc& desc)
{
	BufferDesc bufferDesc;
	bufferDesc.access = desc.access;
	bufferDesc.byteSize = desc.byteSize;
	bufferDesc.name = desc.name;
	bufferDesc.usage = BufferUsage::INDEX;
	bufferDesc.initialState = (TRANSITION_STATES_FLAGS)TRANSITION_STATES::INDEX_BUFFER;
	return bufferDesc;
}
D3D12IndexBuffer::D3D12IndexBuffer(D3D12Device* device, IndexBufferDesc desc) : m_buffer(device, ConvertIndexDesc(desc)), m_desc(desc)
{
	m_view.BufferLocation = m_buffer.Resource()->GetGPUVirtualAddress();
	m_view.SizeInBytes = desc.byteSize;
	m_view.Format = D3D12Format(desc.format);
}

IndexBufferDesc D3D12IndexBuffer::GetDesc()
{
	return m_desc;
}

IBuffer* D3D12IndexBuffer::GetBuffer()
{
	return &m_buffer;
}

std::shared_ptr<FrameBufferMemory> FrameBufferMemory::Create(D3D12Device* device, size_t size) 
{
	return std::shared_ptr<FrameBufferMemory>(new FrameBufferMemory(device, size));
}

FrameBufferMemory::FrameBufferMemory(D3D12Device* device, size_t size) : m_device(device)
{
	//Align the size to fit into the ALIGNMENT
	size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC bufferDesc = {};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = size;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	VerifySuccess(device->Device()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		IID_PPV_ARGS(&m_buffer)));

	VerifySuccess(m_buffer->Map(0, nullptr, &m_cpuVirtualAddres));


	m_bufferSize = size;
	m_gpuVirtualAddress = m_buffer->GetGPUVirtualAddress();
}

FrameBufferMemory::~FrameBufferMemory()
{
	if (m_buffer && m_cpuVirtualAddres)
	{
		m_buffer->Unmap(0, nullptr);
		m_cpuVirtualAddres = nullptr;
	}
}

bool FrameBufferMemory::Allocate(size_t size, ID3D12Resource** pBuffer, size_t* pOffset, void** pCpuVA, D3D12_GPU_VIRTUAL_ADDRESS* pGpuVA, uint32_t alignment, DescriptorIndex& descriptorIndex)
{
	size_t dest = (m_freeDataLocation + alignment - 1) & ~(alignment - 1);

	INF_ASSERT(dest + size <= m_bufferSize, "Failed to allocate buffer inside FrameBufferMemory, not enough memory");
	if (dest + size > m_bufferSize)
		return false;

	if (pCpuVA) 
		*pCpuVA = (char*)m_cpuVirtualAddres + dest;
	if (pGpuVA) 
		*pGpuVA = m_gpuVirtualAddress + dest;
	if (pBuffer) 
		*pBuffer = m_buffer.Get();
	if (pOffset) 
		*pOffset = dest;

	m_descriptorIndices.push_back(m_device->SRVDescriptoHeap().AllocateDescriptor());
	descriptorIndex = m_descriptorIndices.back();

	m_freeDataLocation = dest + size;
	return true;
}

void FrameBufferMemory::SetName(const wchar_t* name)
{
	if (m_buffer)
		m_buffer->SetName(name);
}

void FrameBufferMemory::Reset()
{
	m_freeDataLocation = 0;

	for (DescriptorIndex index : m_descriptorIndices)
	{
		m_device->SRVDescriptoHeap().ReleaseDescriptor(index);
	}
	m_descriptorIndices.clear();
}
