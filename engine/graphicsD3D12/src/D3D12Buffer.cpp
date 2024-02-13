#include "infPCH.h"
#include "graphics/d3d12/D3D12Buffer.h"
#include "core/Assert.h"
#include "graphics/d3d12/D3D12Device.h"
#include "graphics/d3d12/D3D12Defines.h"

using namespace INF::GFX;

D3D12Buffer::D3D12Buffer(D3D12Device* device, const BufferDesc& desc) : m_device(device), m_desc(desc)
{
	constexpr uint32_t CONSTANT_BUFFER_ALINGMENT = 255;

	D3D12_RESOURCE_DESC bufferDesc = {};
	switch (desc.usage)
	{
	case BufferUsage::GENERIC:
	case BufferUsage::VERTEX:
	case BufferUsage::INDEX:
		bufferDesc.Width = desc.byteSize;
		break;
	case BufferUsage::CONSTANT:
		bufferDesc.Width = desc.byteSize + (CONSTANT_BUFFER_ALINGMENT - (desc.byteSize % CONSTANT_BUFFER_ALINGMENT)); //Constant buffers are 256 alligned
		break;
	default:
		INF_ASSERT(false, "Usage not supported");
		break;
	}

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

	VerifySuccess(m_device->Device()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		initialState,
		nullptr,
		IID_PPV_ARGS(&m_resource)));
}

BufferDesc D3D12Buffer::GetDesc()
{
	return m_desc;
}
