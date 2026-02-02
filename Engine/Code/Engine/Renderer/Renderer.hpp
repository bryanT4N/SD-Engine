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
class VertexBuffer;

//-----------------------------------------------------------------------------------------------
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;

//-----------------------------------------------------------------------------------------------
enum class BlendMode
{
	ALPHA,
	ADDITIVE,
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
	VertexBuffer*				m_immediateVBO				= nullptr;

public:
	Renderer(RenderConfig const& config);
	~Renderer();
	void		Startup(); 
	void		Shutdown();
	void		BeginFrame() const;
	void		EndFrame() const;

	void		ClearScreen(Rgba8 const& clearColor) const;

	void		BeginCamera([[maybe_unused]] Camera const& camera) const;
	void		EndCamera(Camera const& camera) const;

	void		DrawVertexArray(int numVertexes, Vertex const* vertexes) const;
	void		DrawVertexArray(const std::vector<Vertex>& vertexes) const;
	VertexBuffer* CreateVertexBuffer(unsigned int size, unsigned int stride) const;
	void		CopyCPUToGPU(const void* data, unsigned int size, VertexBuffer* vbo) const;
	void		BindVertexBuffer(VertexBuffer* vbo) const;
	void		DrawVertexBuffer(VertexBuffer* vbo, unsigned int vertexCount) const;

	void		BindTexture(Texture* texture);
	void		SetBlendMode( BlendMode blendMode ) const;
	Shader*		CreateShader(char const* shaderName, char const* shaderSource);
	bool		CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name,
					char const* source, char const* entryPoint, char const* target);
	void		BindShader(Shader* shader);

	Texture*	CreateOrGetTextureFromFile(char const* imageFilePath);
	Texture*	GetTextureFromFileName(char const* imageFilePath);

	BitmapFont* CreateOrGetBitmapFont(char const* bitmapFontFilePathWithNoExtension);

private:
	Texture*	CreateTextureFromFile(char const* imageFilePath);
	Texture*	CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData);
	BitmapFont*	CreateBitmapFont(char const* bitmapFontFilePathWithNoExtension);
};