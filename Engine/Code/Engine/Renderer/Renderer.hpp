#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <vector>

#define DX_SAFE_RELEASE(dxObject)	\
	do								\
	{								\
		if ((dxObject) != nullptr)	\
		{							\
			(dxObject)->Release();	\
			(dxObject) = nullptr;	\
		}							\
	} while (0)

//-----------------------------------------------------------------------------------------------
struct Rgba8;
struct Vertex;
class Camera;
class Texture;
class BitmapFont;
class Shader;
class Image;
class VertexBuffer;
class ConstantBuffer;

//-----------------------------------------------------------------------------------------------
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11BlendState;
struct ID3D11SamplerState;

//-----------------------------------------------------------------------------------------------
#if defined(OPAQUE)
#undef OPAQUE
#endif

enum class BlendMode
{
	OPAQUE,
	ALPHA,
	ADDITIVE,
	COUNT,
};

enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	COUNT,
};

//-----------------------------------------------------------------------------------------------
struct RenderConfig {
	bool m_isEnabled = true;
};

//-----------------------------------------------------------------------------------------------
class Renderer
{
public:
	RenderConfig				m_config;
	std::vector<Texture*>		m_loadedTextures;
	std::vector<BitmapFont*>	m_loadedFonts;

protected:
	ID3D11Device*				m_device					= nullptr;
	ID3D11DeviceContext*		m_deviceContext				= nullptr;
	IDXGISwapChain*				m_swapChain					= nullptr;
	ID3D11RasterizerState*		m_rasterizerState			= nullptr;
	ID3D11RenderTargetView*		m_renderTargetView			= nullptr;
#if defined(ENGINE_DEBUG_RENDER)
	void*						m_dxgiDebug					= nullptr;
	void*						m_dxgiDebugModule			= nullptr;
#endif
	std::vector<Shader*>		m_loadedShaders;
	Shader*						m_currentShader				= nullptr;
	Shader*						m_defaultShader				= nullptr;
	VertexBuffer*				m_immediateVBO				= nullptr;
	ConstantBuffer*				m_cameraCBO					= nullptr;

	ID3D11BlendState*			m_blendState				= nullptr;
	BlendMode					m_desiredBlendMode			= BlendMode::ALPHA;
	ID3D11BlendState*			m_blendStates[(int)(BlendMode::COUNT)] = {};

	ID3D11SamplerState*			m_samplerState				= nullptr;
	SamplerMode					m_desiredSamplerMode		= SamplerMode::POINT_CLAMP;
	ID3D11SamplerState*			m_samplerStates[(int)(SamplerMode::COUNT)] = {};

	const Texture*				m_currentTexture			= nullptr;
	const Texture*				m_defaultTexture			= nullptr;

public:
	Renderer(RenderConfig const& config);
	~Renderer();
	void		Startup(); 
	void		Shutdown();
	void		BeginFrame() const;
	void		EndFrame() const;

	void		ClearScreen(Rgba8 const& clearColor) const;

	void		BeginCamera(Camera const& camera);
	void		EndCamera(Camera const& camera) const;

	void		DrawVertexArray(int numVertexes, Vertex const* vertexes);
	void		DrawVertexArray(const std::vector<Vertex>& vertexes);
	VertexBuffer* CreateVertexBuffer(unsigned int size, unsigned int stride) const;
	void		CopyCPUToGPU(const void* data, unsigned int size, VertexBuffer* vbo) const;
	void		BindVertexBuffer(VertexBuffer* vbo) const;
	void		DrawVertexBuffer(VertexBuffer* vbo, unsigned int vertexCount);

	void		BindTexture(const Texture* texture);
	void		SetBlendMode(BlendMode blendMode);
	void		SetSamplerMode(SamplerMode samplerMode);
	void		SetStatesIfChanged();

	ConstantBuffer* CreateConstantBuffer(const unsigned int size);
	void		CopyCPUToGPU(const void* data, unsigned int size, ConstantBuffer* cbo);
	void		BindConstantBuffer(int slot, ConstantBuffer* cbo);
	Shader*		CreateShader(char const* shaderName, char const* shaderSource);
	Shader*		CreateShader(char const* shaderName);
	bool		CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name,
					char const* source, char const* entryPoint, char const* target);
	void		BindShader(Shader* shader);

	Texture*	CreateOrGetTextureFromFile(char const* imageFilePath);
	Texture*	GetTextureFromFileName(char const* imageFilePath);

	BitmapFont* CreateOrGetBitmapFont(char const* bitmapFontFilePathWithNoExtension);

private:
	Texture*	CreateTextureFromFile(char const* imageFilePath);
	Texture*	CreateTextureFromImage(const Image& image);
	BitmapFont*	CreateBitmapFont(char const* bitmapFontFilePathWithNoExtension);
};