#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <d3d11.h>


Texture::Texture()
	: m_name(""), m_dimensions(0, 0)
{
}


Texture::~Texture()
{
	DX_SAFE_RELEASE(m_shaderResourceView);
	DX_SAFE_RELEASE(m_texture);
}
