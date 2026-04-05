#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <d3d11.h>

//-----------------------------------------------------------------------------------------------
IndexBuffer::IndexBuffer(ID3D11Device* device, unsigned int size, unsigned int stride)
	: m_device(device)
	, m_size(size)
	, m_stride(stride)
{
	Create();
}

//-----------------------------------------------------------------------------------------------
IndexBuffer::~IndexBuffer()
{
	DX_SAFE_RELEASE(m_buffer);
}

//-----------------------------------------------------------------------------------------------
void IndexBuffer::Create()
{
	D3D11_BUFFER_DESC bufferDesc = { };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = m_size;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, &m_buffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create index buffer.");
	}
}

//-----------------------------------------------------------------------------------------------
void IndexBuffer::Resize(unsigned int size)
{
	if (size == m_size) {
		return;
	}

	m_size = size;
	DX_SAFE_RELEASE(m_buffer);
	Create();
}

//-----------------------------------------------------------------------------------------------
unsigned int IndexBuffer::GetSize()
{
	return m_size;
}

//-----------------------------------------------------------------------------------------------
unsigned int IndexBuffer::GetStride()
{
	return m_stride;
}

//-----------------------------------------------------------------------------------------------
unsigned int IndexBuffer::GetCount()
{
	if (m_stride == 0) {
		return 0;
	}
	return m_size / m_stride;
}
