#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <d3d11.h>

//-----------------------------------------------------------------------------------------------
Shader::Shader(const ShaderConfig& config)
	: m_config(config)
{
}

//-----------------------------------------------------------------------------------------------
Shader::~Shader()
{
	DX_SAFE_RELEASE(m_inputLayout);
	DX_SAFE_RELEASE(m_pixelShader);
	DX_SAFE_RELEASE(m_vertexShader);
}

//-----------------------------------------------------------------------------------------------
const std::string& Shader::GetName() const
{
	return m_config.m_name;
}

//-----------------------------------------------------------------------------------------------
VertexType Shader::GetVertexType() const
{
	return m_config.m_vertexType;
}
