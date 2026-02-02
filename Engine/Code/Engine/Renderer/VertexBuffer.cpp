#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <d3d11.h>

//-----------------------------------------------------------------------------------------------
VertexBuffer::VertexBuffer(ID3D11Device* device, unsigned int size, unsigned int stride)
	: m_device(device)
	, m_size(size)
	, m_stride(stride)
{
	Create();
}

//-----------------------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	DX_SAFE_RELEASE(m_buffer);
}

//-----------------------------------------------------------------------------------------------
void VertexBuffer::Create()
{
	D3D11_BUFFER_DESC bufferDesc = { };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = m_size;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, &m_buffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create vertex buffer.");
	}
}

//-----------------------------------------------------------------------------------------------
void VertexBuffer::Resize(unsigned int size)
{
	if (size == m_size) {
		return;
	}

	m_size = size;
	DX_SAFE_RELEASE(m_buffer);
	Create();
}

//-----------------------------------------------------------------------------------------------
unsigned int VertexBuffer::GetSize()
{
	return m_size;
}

//-----------------------------------------------------------------------------------------------
unsigned int VertexBuffer::GetStride()
{
	return m_stride;
}

//-----------------------------------------------------------------------------------------------
unsigned int VertexBuffer::GetCount()
{
	if (m_stride == 0) {
		return 0;
	}
	return m_size / m_stride;
}
