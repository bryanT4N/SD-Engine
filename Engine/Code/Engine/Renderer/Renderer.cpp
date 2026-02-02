#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include <string>
#include <cstring>

#include "ThirdParty/stb/stb_image.h"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <Windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

#if defined(ENGINE_DEBUG_RENDER)
#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")
#endif

//-----------------------------------------------------------------------------------------------
static char const* g_defaultShaderSource = R"(
struct vs_input_t
{
	float3 localPosition : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct v2p_t
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

v2p_t VertexMain(vs_input_t input)
{
	v2p_t v2p;
	v2p.position = float4(input.localPosition, 1);
	v2p.color = input.color;
	v2p.uv = input.uv;
	return v2p;
}

float4 PixelMain(v2p_t input) :SV_Target0
{
	return float4(input.color);
}
)";

//-----------------------------------------------------------------------------------------------
Renderer::Renderer(RenderConfig const& config)
	:m_config(config)
{
}

//-----------------------------------------------------------------------------------------------
Renderer::~Renderer()
{
}

//-----------------------------------------------------------------------------------------------
void Renderer::Startup()
{

	unsigned int deviceFlags = 0;

#if defined(ENGINE_DEBUG_RENDER)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create debug module
#if defined(ENGINE_DEBUG_RENDER)
	m_dxgiDebugModule = (void*)::LoadLibraryA("dxgidebug.dll");
	if (m_dxgiDebugModule == nullptr) {
		ERROR_AND_DIE("Could not load dxgidebug.dl1.");
	}

	typedef HRESULT(WINAPI* GetDebugModuleCB)(REFIID, void**);
	((GetDebugModuleCB)::GetProcAddress((HMODULE)m_dxgiDebugModule, "DXGIGetDebugInterface"))
		(__uuidof(IDXGIDebug), &m_dxgiDebug);

	if (m_dxgiDebug == nullptr)
	{
		ERROR_AND_DIE("Could not load debug module.");
	}
#endif

	// CREATE THE DEVICE AND SWAP CHAIN
	// Create device and swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	swapChainDesc.BufferDesc.Width = g_engine->m_window->GetClientDimensions().x;
	swapChainDesc.BufferDesc.Height = g_engine->m_window->GetClientDimensions().y;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = (HWND)g_engine->m_window->GetHwnd();
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags,
		nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&m_swapChain, &m_device, nullptr, &m_deviceContext);

	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create D3D 11 device and swap chain.");
	}

	// SAVE BACK BUFFER VIEW
	// Get back buffer texture
	ID3D11Texture2D* backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not get swap chain buffer.");
	}

	hr = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could create render target view for swap chain buffer.");
	}

	backBuffer->Release();


	// Set viewport
	D3D11_VIEWPORT viewport = { };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (float)g_engine->m_window->GetClientDimensions().x;
	viewport.Height = (float)g_engine->m_window->GetClientDimensions().y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_deviceContext->RSSetViewports(1, &viewport);

	// Set rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc = { };
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create rasterizer state.");
	}

	m_deviceContext->RSSetState(m_rasterizerState);

	Shader* defaultShader = CreateShader("Default", g_defaultShaderSource);
	BindShader(defaultShader);

	m_immediateVBO = CreateVertexBuffer(sizeof(Vertex_PCU), sizeof(Vertex_PCU));
}

//-----------------------------------------------------------------------------------------------
void Renderer::Shutdown()
{
	for (int shaderIndex = 0; shaderIndex < static_cast<int>(m_loadedShaders.size()); ++shaderIndex) {
		delete m_loadedShaders[shaderIndex];
		m_loadedShaders[shaderIndex] = nullptr;
	}
	m_loadedShaders.clear();
	m_currentShader = nullptr;

	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	DX_SAFE_RELEASE(m_rasterizerState);
//	DX_SAFE_RELEASE(m_vertexBuffer);
//	DX_SAFE_RELEASE(m_vertexShader);
//	DX_SAFE_RELEASE(m_pixelShader);
//	DX_SAFE_RELEASE(m_inputLayoutForVertex_PCU);
	DX_SAFE_RELEASE(m_renderTargetView);
	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_deviceContext);
	DX_SAFE_RELEASE(m_device);

	// Report error leaks and release debug module
#if defined(ENGINE_DEBUG_RENDER)
	((IDXGIDebug*)m_dxgiDebug)->ReportLiveObjects(
		DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
	);

	((IDXGIDebug*)m_dxgiDebug)->Release();
	m_dxgiDebug = nullptr;

	::FreeLibrary((HMODULE)m_dxgiDebugModule);
	m_dxgiDebugModule = nullptr;
#endif

}

//-----------------------------------------------------------------------------------------------
void Renderer::BeginFrame() const
{
	if (m_deviceContext == nullptr || m_renderTargetView == nullptr) {
		return;
	}

	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
}

//-----------------------------------------------------------------------------------------------
void Renderer::EndFrame() const
{
	// Present
	HRESULT hr;
	hr = m_swapChain->Present(0, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		ERROR_AND_DIE("Device has been lost, application will now terminate.");
	}
}

//-----------------------------------------------------------------------------------------------
void Renderer::ClearScreen(Rgba8 const& clearColor) const
{
	// Clear the screen
	float colorAsFloats[4];
	clearColor.GetAsFloats(colorAsFloats);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, colorAsFloats);
}

//-----------------------------------------------------------------------------------------------
void Renderer::BeginCamera([[maybe_unused]] Camera const& camera) const
{
	D3D11_VIEWPORT viewport = { };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (float)g_engine->m_window->GetClientDimensions().x;
	viewport.Height = (float)g_engine->m_window->GetClientDimensions().y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_deviceContext->RSSetViewports(1, &viewport);
}

//-----------------------------------------------------------------------------------------------
void Renderer::EndCamera( [[maybe_unused]] Camera const& camera) const
{
}

//-----------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray(int numVertexes, Vertex const* vertexes) const
{
	if (m_immediateVBO == nullptr || vertexes == nullptr || numVertexes <= 0) {
		return;
	}

	unsigned int dataSize = static_cast<unsigned int>(numVertexes) * sizeof(Vertex);
	CopyCPUToGPU(vertexes, dataSize, m_immediateVBO);
	DrawVertexBuffer(m_immediateVBO, static_cast<unsigned int>(numVertexes));
}

void Renderer::DrawVertexArray(const std::vector<Vertex>& vertexes) const
{
	DrawVertexArray(static_cast<int>(vertexes.size()), vertexes.data());
}

//------------------------------------------------------------------------------------------------
VertexBuffer* Renderer::CreateVertexBuffer(unsigned int size, unsigned int stride) const
{
	VertexBuffer* vbo = new VertexBuffer(m_device, size, stride);
	return vbo;
}

//------------------------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU(const void* data, unsigned int size, VertexBuffer* vbo) const
{
	if (vbo == nullptr || data == nullptr || size == 0) {
		return;
	}

	if (vbo->GetSize() < size) {
		vbo->Resize(size);
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(vbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(vbo->m_buffer, 0);
}

//------------------------------------------------------------------------------------------------
void Renderer::BindVertexBuffer(VertexBuffer* vbo) const
{
	if (vbo == nullptr) {
		return;
	}

	UINT stride = vbo->GetStride();
	UINT startOffset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &vbo->m_buffer, &stride, &startOffset);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//------------------------------------------------------------------------------------------------
void Renderer::DrawVertexBuffer(VertexBuffer* vbo, unsigned int vertexCount) const
{
	if (vbo == nullptr || vertexCount == 0) {
		return;
	}

	BindVertexBuffer(vbo);
	m_deviceContext->Draw(vertexCount, 0);
}

//------------------------------------------------------------------------------------------------
Shader* Renderer::CreateShader(char const* shaderName, char const* shaderSource)
{
	if (shaderName == nullptr || shaderSource == nullptr) {
		ERROR_AND_DIE("CreateShader received null input.");
	}

	for (int shaderIndex = 0; shaderIndex < static_cast<int>(m_loadedShaders.size()); ++shaderIndex) {
		Shader* shader = m_loadedShaders[shaderIndex];
		if (shader && shader->GetName() == shaderName) {
			return shader;
		}
	}

	ShaderConfig config;
	config.m_name = shaderName;
	Shader* newShader = new Shader(config);

	std::vector<unsigned char> vertexByteCode;
	bool vertexCompiled = CompileShaderToByteCode(vertexByteCode, shaderName, shaderSource,
		config.m_vertexEntryPoint.c_str(), "vs_5_0");
	if (!vertexCompiled) {
		delete newShader;
		return nullptr;
	}

	HRESULT hr = m_device->CreateVertexShader(
		vertexByteCode.data(),
		vertexByteCode.size(),
		nullptr,
		&newShader->m_vertexShader);
	if (!SUCCEEDED(hr)) {
		delete newShader;
		ERROR_AND_DIE("Could not create vertex shader.");
	}

	std::vector<unsigned char> pixelByteCode;
	bool pixelCompiled = CompileShaderToByteCode(pixelByteCode, shaderName, shaderSource,
		config.m_pixelEntryPoint.c_str(), "ps_5_0");
	if (!pixelCompiled) {
		delete newShader;
		return nullptr;
	}

	hr = m_device->CreatePixelShader(
		pixelByteCode.data(),
		pixelByteCode.size(),
		nullptr,
		&newShader->m_pixelShader);
	if (!SUCCEEDED(hr)) {
		delete newShader;
		ERROR_AND_DIE("Could not create pixel shader.");
	}

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM,
			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
			0, D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(inputElementDesc);
	hr = m_device->CreateInputLayout(
		inputElementDesc, numElements,
		vertexByteCode.data(),
		vertexByteCode.size(),
		&newShader->m_inputLayout);
	if (!SUCCEEDED(hr)) {
		delete newShader;
		ERROR_AND_DIE("Could not create input layout.");
	}

	m_loadedShaders.push_back(newShader);
	return newShader;
}

//------------------------------------------------------------------------------------------------
bool Renderer::CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name,
	char const* source, char const* entryPoint, char const* target)
{
	DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(ENGINE_DEBUG_RENDER)
	shaderFlags = D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	shaderFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompile(
		source, strlen(source),
		name, nullptr, nullptr,
		entryPoint, target, shaderFlags,
		0, &shaderBlob, &errorBlob);

	if (SUCCEEDED(hr))
	{
		outByteCode.resize(shaderBlob->GetBufferSize());
		memcpy(
			outByteCode.data(),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize());
	}
	else
	{
		if (errorBlob != nullptr) {
			DebuggerPrintf((char*)errorBlob->GetBufferPointer());
		}
		ERROR_AND_DIE(Stringf("Could not compile shader: %s", name));
	}

	shaderBlob->Release();
	if (errorBlob != nullptr) {
		errorBlob->Release();
	}

	return SUCCEEDED(hr);
}

//------------------------------------------------------------------------------------------------
void Renderer::BindShader(Shader* shader)
{
	if (shader == nullptr) {
		return;
	}

	m_currentShader = shader;
	m_deviceContext->VSSetShader(shader->m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(shader->m_pixelShader, nullptr, 0);
	m_deviceContext->IASetInputLayout(shader->m_inputLayout);
}

//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetTextureFromFile(char const* imageFilePath)
{
	// See if we already have this texture previously loaded
	Texture* existingTexture = GetTextureFromFileName(imageFilePath); // You need to write this
	if (existingTexture)
	{
		return existingTexture;
	}

	// Never seen this texture before!  Let's load it.
	Texture* newTexture = CreateTextureFromFile(imageFilePath);
	return newTexture;
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromFile(char const* imageFilePath)
{
	IntVec2 dimensions = IntVec2(0, 0);		// This will be filled in for us to indicate image width & height
	int bytesPerTexel = 0;					// ...and how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)

	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load(1); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* texelData = stbi_load(imageFilePath, &dimensions.x, &dimensions.y, &bytesPerTexel, 0);

	// Check if the load was successful
	GUARANTEE_OR_DIE(texelData, Stringf("Failed to load image \"%s\"", imageFilePath));

	Texture* newTexture = CreateTextureFromData(imageFilePath, dimensions, bytesPerTexel, texelData);

	// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	stbi_image_free(texelData);

	return newTexture;
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData)
{
	// Check if the load was successful
	GUARANTEE_OR_DIE(texelData, Stringf("CreateTextureFromData failed for \"%s\" - texelData was null!", name));
	GUARANTEE_OR_DIE(bytesPerTexel >= 3 && bytesPerTexel <= 4, Stringf("CreateTextureFromData failed for \"%s\" - unsupported BPP=%i (must be 3 or 4)", name, bytesPerTexel));
	GUARANTEE_OR_DIE(dimensions.x > 0 && dimensions.y > 0, Stringf("CreateTextureFromData failed for \"%s\" - illegal texture dimensions (%i x %i)", name, dimensions.x, dimensions.y));

	Texture* newTexture = new Texture();
	newTexture->m_name = name; // NOTE: m_name must be a std::string, otherwise it may point to temporary data!
	newTexture->m_dimensions = dimensions;

	m_loadedTextures.push_back(newTexture);
	return newTexture;
}

Texture* Renderer::GetTextureFromFileName(char const* imageFilePath)
{
	if (imageFilePath == nullptr) return nullptr;

	std::string targetName = imageFilePath;

	for (int indexTexture = 0; indexTexture < static_cast<int>(m_loadedTextures.size()); ++indexTexture) {
		Texture* tex = m_loadedTextures[indexTexture];
		if (m_loadedTextures[indexTexture]->GetImageFilePath() == imageFilePath)
			return tex;
	}

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
void Renderer::BindTexture(Texture* texture)
{
	if (texture)
	{
	}
	else
	{
	}
}

//-----------------------------------------------------------------------------------------------
void Renderer::SetBlendMode( BlendMode blendMode ) const
{
	if( blendMode == BlendMode::ALPHA )
	{
	}
	else if( blendMode == BlendMode::ADDITIVE )
	{
	}
	else
	{
		ERROR_AND_DIE( Stringf( "Unknown / unsupported blend mode #%i", static_cast<int>( blendMode ) ) );
	}
}

//-----------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateOrGetBitmapFont(char const* bitmapFontFilePathWithNoExtension)
{
	if (bitmapFontFilePathWithNoExtension == nullptr)
	{
		return nullptr;
	}

	// See if we already have this font previously loaded
	int numFonts = static_cast<int>(m_loadedFonts.size());
	for (int fontIndex = 0; fontIndex < numFonts; ++fontIndex)
	{
		BitmapFont* existingFont = m_loadedFonts[fontIndex];
		if (existingFont != nullptr &&
			existingFont->m_fontFilePathNameWithNoExtension == bitmapFontFilePathWithNoExtension)
		{
			return existingFont;
		}
	}

	BitmapFont* newFont = CreateBitmapFont(bitmapFontFilePathWithNoExtension);
	m_loadedFonts.push_back(newFont);
	return newFont;
}

//------------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateBitmapFont(char const* bitmapFontFilePathWithNoExtension)
{
	if (bitmapFontFilePathWithNoExtension == nullptr)
	{
		return nullptr;
	}

	std::string baseName = bitmapFontFilePathWithNoExtension;
	std::string texturePath = baseName + ".png";

	Texture* fontTexture = CreateOrGetTextureFromFile(texturePath.c_str());
	GUARANTEE_OR_DIE(fontTexture != nullptr, Stringf("Failed to load bitmap font texture \"%s\"", texturePath.c_str()));

	BitmapFont* newFont = new BitmapFont(bitmapFontFilePathWithNoExtension, *fontTexture);
	return newFont;
}