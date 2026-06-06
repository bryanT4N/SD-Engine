#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"

#include <string>
#include <cstring>
#include <cstdio>

#include "ThirdParty/stb/stb_image.h"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <Windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)
#if defined(OPAQUE)
#undef OPAQUE
#endif

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

#include "Engine/Renderer/DefaultShader.hpp"

//-----------------------------------------------------------------------------------------------
struct CameraConstantsOnCPU
{
	Mat44 WorldToCameraTransform;		// View transform
	Mat44 CameraToRenderTransform;		// Non-standard transform from game to DirectX conventions
	Mat44 RenderToClipTransform;		// Projection transform
};

static const int k_cameraConstantsSlot = 2;

//-----------------------------------------------------------------------------------------------
struct ModelConstantsOnCPU
{
	Mat44 ModelToWorldTransform;		// Model transform
	float ModelColor[4];
};

static const int k_modelConstantsSlot = 3;

//-----------------------------------------------------------------------------------------------
static bool DoesFileExist(char const* filepath)
{
	if (filepath == nullptr) {
		return false;
	}

	FILE* file = nullptr;
	errno_t fileError = fopen_s(&file, filepath, "rb");
	if (fileError != 0 || file == nullptr) {
		return false;
	}

	fclose(file);
	return true;
}

// Winding is evaluated after CameraToRenderTransform.
static constexpr bool FRONT_FACE_IS_COUNTER_CLOCKWISE_IN_RENDER_SPACE = true;

static D3D11_FILL_MODE GetFillModeForRasterizerMode(RasterizerMode rasterizerMode)
{
	switch (rasterizerMode)
	{
	case RasterizerMode::SOLID_CULL_NONE:
	case RasterizerMode::SOLID_CULL_BACK:
		return D3D11_FILL_SOLID;
	case RasterizerMode::WIREFRAME_CULL_NONE:
	case RasterizerMode::WIREFRAME_CULL_BACK:
		return D3D11_FILL_WIREFRAME;
	case RasterizerMode::COUNT:
	default:
		return D3D11_FILL_SOLID;
	}
}

static D3D11_CULL_MODE GetCullModeForRasterizerMode(RasterizerMode rasterizerMode)
{
	switch (rasterizerMode)
	{
	case RasterizerMode::SOLID_CULL_NONE:
	case RasterizerMode::WIREFRAME_CULL_NONE:
		return D3D11_CULL_NONE;
	case RasterizerMode::SOLID_CULL_BACK:
	case RasterizerMode::WIREFRAME_CULL_BACK:
		return D3D11_CULL_BACK;
	case RasterizerMode::COUNT:
	default:
		return D3D11_CULL_BACK;
	}
}

static char const* GetRasterizerModeName(RasterizerMode rasterizerMode)
{
	switch (rasterizerMode)
	{
	case RasterizerMode::SOLID_CULL_NONE:
		return "RasterizerMode::SOLID_CULL_NONE";
	case RasterizerMode::SOLID_CULL_BACK:
		return "RasterizerMode::SOLID_CULL_BACK";
	case RasterizerMode::WIREFRAME_CULL_NONE:
		return "RasterizerMode::WIREFRAME_CULL_NONE";
	case RasterizerMode::WIREFRAME_CULL_BACK:
		return "RasterizerMode::WIREFRAME_CULL_BACK";
	case RasterizerMode::COUNT:
	default:
		return "RasterizerMode::COUNT";
	}
}

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

	// Create depth stencil texture and view
	D3D11_TEXTURE2D_DESC depthTextureDesc = { };
	depthTextureDesc.Width = g_engine->m_window->GetClientDimensions().x;
	depthTextureDesc.Height = g_engine->m_window->GetClientDimensions().y;
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTextureDesc.SampleDesc.Count = 1;

	hr = m_device->CreateTexture2D(&depthTextureDesc, nullptr, &m_depthStencilTexture);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create texture for depth stencil.");
	}

	hr = m_device->CreateDepthStencilView(m_depthStencilTexture, nullptr, &m_depthStencilDSV);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create depth stencil view.");
	}


	// Set viewport
	D3D11_VIEWPORT viewport = { };
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (float)g_engine->m_window->GetClientDimensions().x;
	viewport.Height = (float)g_engine->m_window->GetClientDimensions().y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_deviceContext->RSSetViewports(1, &viewport);

	// Rasterizer states
	D3D11_RASTERIZER_DESC rasterizerDesc = { };
	rasterizerDesc.FrontCounterClockwise = FRONT_FACE_IS_COUNTER_CLOCKWISE_IN_RENDER_SPACE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	for (int modeIndex = 0; modeIndex < (int)RasterizerMode::COUNT; ++modeIndex) {
		RasterizerMode mode = static_cast<RasterizerMode>(modeIndex);
		rasterizerDesc.FillMode = GetFillModeForRasterizerMode(mode);
		rasterizerDesc.CullMode = GetCullModeForRasterizerMode(mode);
		hr = m_device->CreateRasterizerState(
			&rasterizerDesc,
			&m_rasterizerStates[modeIndex]);
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE(Stringf("CreateRasterizerState failed for %s", GetRasterizerModeName(mode)));
		}
	}

	m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_NONE;
	m_rasterizerState = m_rasterizerStates[(int)m_desiredRasterizerMode];
	m_deviceContext->RSSetState(m_rasterizerState);

	std::string defaultShaderPath = "Data/Shaders/Default.hlsl";
	std::string defaultShaderSourceFromFile;
	if (DoesFileExist(defaultShaderPath.c_str())) {
		int bytesRead = FileReadToString(defaultShaderSourceFromFile, defaultShaderPath);
		if (bytesRead > 0) {
			m_defaultShader = CreateShader("Default", defaultShaderSourceFromFile.c_str());
		}
	}

	if (m_defaultShader == nullptr) {
		m_defaultShader = CreateShader("Default", g_defaultShaderSource);
	}

	GUARANTEE_OR_DIE(m_defaultShader != nullptr, "Could not create default shader.");
	BindShader(m_defaultShader);

	m_immediateVBO = CreateVertexBuffer(sizeof(Vertex_PCU), sizeof(Vertex_PCU));
	m_cameraCBO = CreateConstantBuffer(sizeof(CameraConstantsOnCPU));
	m_modelCBO = CreateConstantBuffer(sizeof(ModelConstantsOnCPU));

	// Blend states
	D3D11_BLEND_DESC blendDesc = { };
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// OPAQUE
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)(BlendMode::OPAQUE)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::OPAQUE failed.");
	}

	// ALPHA
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)(BlendMode::ALPHA)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::ALPHA failed.");
	}

	// ADDITIVE
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)(BlendMode::ADDITIVE)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::ADDITIVE failed.");
	}

	m_desiredBlendMode = BlendMode::ALPHA;

	// Sampler states
	D3D11_SAMPLER_DESC samplerDesc = { };
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// POINT_CLAMP
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = m_device->CreateSamplerState(&samplerDesc,
		&m_samplerStates[(int)SamplerMode::POINT_CLAMP]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateSamplerState for SamplerMode::POINT_CLAMP failed.");
	}

	// BILINEAR_WRAP
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = m_device->CreateSamplerState(&samplerDesc,
		&m_samplerStates[(int)SamplerMode::BILINEAR_WRAP]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateSamplerState for SamplerMode::BILINEAR_WRAP failed.");
	}

	m_desiredSamplerMode = SamplerMode::POINT_CLAMP;

	// Depth stencil states
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { };
	hr = m_device->CreateDepthStencilState(
		&depthStencilDesc,
		&m_depthStencilStates[(int)DepthMode::DISABLED]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateDepthStencilState for DepthMode::DISABLED failed.");
	}

	depthStencilDesc.DepthEnable = TRUE;

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	hr = m_device->CreateDepthStencilState(
		&depthStencilDesc,
		&m_depthStencilStates[(int)DepthMode::READ_ONLY_ALWAYS]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateDepthStencilState for DepthMode::READ_ONLY_ALWAYS failed.");
	}

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = m_device->CreateDepthStencilState(
		&depthStencilDesc,
		&m_depthStencilStates[(int)DepthMode::READ_ONLY_LESS_EQUAL]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateDepthStencilState for DepthMode::READ_ONLY_LESS_EQUAL failed.");
	}

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = m_device->CreateDepthStencilState(
		&depthStencilDesc,
		&m_depthStencilStates[(int)DepthMode::READ_WRITE_LESS_EQUAL]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateDepthStencilState for DepthMode::READ_WRITE_LESS_EQUAL failed.");
	}

	m_desiredDepthMode = DepthMode::DISABLED;

	// Default texture (2x2 white)
	Image defaultImage(IntVec2(2, 2), Rgba8::WHITE);
	m_defaultTexture = CreateTextureFromImage(defaultImage);

	BindTexture(m_defaultTexture);
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

	delete m_cameraCBO;
	m_cameraCBO = nullptr;

	delete m_modelCBO;
	m_modelCBO = nullptr;

	for (int i = 0; i < (int)BlendMode::COUNT; ++i)
	{
		DX_SAFE_RELEASE(m_blendStates[i]);
	}
	m_blendState = nullptr;

	for (int i = 0; i < (int)SamplerMode::COUNT; ++i)
	{
		DX_SAFE_RELEASE(m_samplerStates[i]);
	}
	m_samplerState = nullptr;

	for (int i = 0; i < (int)DepthMode::COUNT; ++i)
	{
		DX_SAFE_RELEASE(m_depthStencilStates[i]);
	}
	m_depthStencilState = nullptr;

	for (int i = 0; i < static_cast<int>(m_loadedTextures.size()); ++i)
	{
		delete m_loadedTextures[i];
	}
	m_loadedTextures.clear();
	m_currentTexture = nullptr;
	m_defaultTexture = nullptr;

	DX_SAFE_RELEASE(m_depthStencilDSV);
	DX_SAFE_RELEASE(m_depthStencilTexture);
	for (int i = 0; i < (int)RasterizerMode::COUNT; ++i)
	{
		DX_SAFE_RELEASE(m_rasterizerStates[i]);
	}
	m_rasterizerState = nullptr;
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

	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilDSV);
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

	if (m_depthStencilDSV != nullptr)
	{
		m_deviceContext->ClearDepthStencilView(
			m_depthStencilDSV,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f,
			0);
	}
}

//-----------------------------------------------------------------------------------------------
void Renderer::BeginCamera(Camera const& camera)
{
	IntVec2 windowDims = g_engine->m_window->GetClientDimensions();
	AABB2 normalizedViewport = camera.GetNormalizedViewport();
	int leftPx = static_cast<int>(normalizedViewport.m_mins.x * (float)windowDims.x + 0.5f);
	int topPx = static_cast<int>((1.f - normalizedViewport.m_maxs.y) * (float)windowDims.y + 0.5f);
	int rightPx = static_cast<int>(normalizedViewport.m_maxs.x * (float)windowDims.x + 0.5f);
	int bottomPx = static_cast<int>((1.f - normalizedViewport.m_mins.y) * (float)windowDims.y + 0.5f);
	D3D11_VIEWPORT viewport = { };
	viewport.TopLeftX = (float)leftPx;
	viewport.TopLeftY = (float)topPx;
	viewport.Width = (float)(rightPx - leftPx);
	viewport.Height = (float)(bottomPx - topPx);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_deviceContext->RSSetViewports(1, &viewport);

	CameraConstantsOnCPU cameraConstantsOnCPU;
	cameraConstantsOnCPU.WorldToCameraTransform = camera.GetWorldToCameraTransform();
	cameraConstantsOnCPU.CameraToRenderTransform = camera.GetCameraToRenderTransform();
	cameraConstantsOnCPU.RenderToClipTransform = camera.GetRenderToClipTransform();
	CopyCPUToGPU(&cameraConstantsOnCPU, sizeof(CameraConstantsOnCPU), m_cameraCBO);
	BindConstantBuffer(k_cameraConstantsSlot, m_cameraCBO);

	SetModelConstants();
}

//-----------------------------------------------------------------------------------------------
void Renderer::EndCamera( [[maybe_unused]] Camera const& camera) const
{
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetModelConstants(
	Mat44 const& modelToWorldTransform,
	Rgba8 const& modelColor)
{
	if (m_modelCBO == nullptr) {
		return;
	}

	ModelConstantsOnCPU modelConstantsOnCPU;
	modelConstantsOnCPU.ModelToWorldTransform = modelToWorldTransform;

	float modelColorAsFloats[4];
	modelColor.GetAsFloats(modelColorAsFloats);
	modelConstantsOnCPU.ModelColor[0] = modelColorAsFloats[0];
	modelConstantsOnCPU.ModelColor[1] = modelColorAsFloats[1];
	modelConstantsOnCPU.ModelColor[2] = modelColorAsFloats[2];
	modelConstantsOnCPU.ModelColor[3] = modelColorAsFloats[3];

	CopyCPUToGPU(&modelConstantsOnCPU, sizeof(ModelConstantsOnCPU), m_modelCBO);
	BindConstantBuffer(k_modelConstantsSlot, m_modelCBO);
}

//-----------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray(int numVertexes, Vertex const* vertexes)
{
	if (m_immediateVBO == nullptr || vertexes == nullptr || numVertexes <= 0) {
		return;
	}

	unsigned int dataSize = static_cast<unsigned int>(numVertexes) * sizeof(Vertex);
	CopyCPUToGPU(vertexes, dataSize, m_immediateVBO);
	DrawVertexBuffer(m_immediateVBO, static_cast<unsigned int>(numVertexes));
}

void Renderer::DrawVertexArray(const std::vector<Vertex>& vertexes)
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
IndexBuffer* Renderer::CreateIndexBuffer(unsigned int size, unsigned int stride) const
{
	IndexBuffer* ibo = new IndexBuffer(m_device, size, stride);
	return ibo;
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
void Renderer::CopyCPUToGPU(const void* data, unsigned int size, IndexBuffer* ibo) const
{
	if (ibo == nullptr || data == nullptr || size == 0) {
		return;
	}

	if (ibo->GetSize() < size) {
		ibo->Resize(size);
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(ibo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(ibo->m_buffer, 0);
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
void Renderer::BindIndexBuffer(IndexBuffer* ibo) const
{
	if (ibo == nullptr) {
		return;
	}

	m_deviceContext->IASetIndexBuffer(ibo->m_buffer, DXGI_FORMAT_R32_UINT, 0);
}

//------------------------------------------------------------------------------------------------
void Renderer::DrawVertexBuffer(VertexBuffer* vbo, unsigned int vertexCount)
{
	if (vbo == nullptr || vertexCount == 0) {
		return;
	}

	SetStatesIfChanged();
	BindVertexBuffer(vbo);
	m_deviceContext->Draw(vertexCount, 0);
}

//------------------------------------------------------------------------------------------------
void Renderer::DrawIndexedVertexBuffer(VertexBuffer* vbo, IndexBuffer* ibo, unsigned int indexCount)
{
	if (vbo == nullptr || ibo == nullptr || indexCount == 0) {
		return;
	}

	SetStatesIfChanged();
	BindVertexBuffer(vbo);
	BindIndexBuffer(ibo);
	m_deviceContext->DrawIndexed(indexCount, 0, 0);
}

//------------------------------------------------------------------------------------------------
ConstantBuffer* Renderer::CreateConstantBuffer(const unsigned int size)
{
	ConstantBuffer* cbo = new ConstantBuffer(size);

	D3D11_BUFFER_DESC bufferDesc = { };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, &cbo->m_buffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create constant buffer.");
	}

	return cbo;
}


//------------------------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU(const void* data, unsigned int size, ConstantBuffer* cbo)
{
	if (cbo == nullptr || data == nullptr || size == 0)
	{
		return;
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(cbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(cbo->m_buffer, 0);
}


//------------------------------------------------------------------------------------------------
void Renderer::BindConstantBuffer(int slot, ConstantBuffer* cbo)
{
	m_deviceContext->VSSetConstantBuffers(slot, 1, &cbo->m_buffer);
	m_deviceContext->PSSetConstantBuffers(slot, 1, &cbo->m_buffer);
}


//------------------------------------------------------------------------------------------------
Shader* Renderer::CreateShader(char const* shaderName, char const* shaderSource, VertexType vertexType)
{
	if (shaderName == nullptr || shaderSource == nullptr) {
		ERROR_AND_DIE("CreateShader received null input.");
	}

	for (int shaderIndex = 0; shaderIndex < static_cast<int>(m_loadedShaders.size()); ++shaderIndex) {
		Shader* shader = m_loadedShaders[shaderIndex];
		if (shader && shader->GetName() == shaderName && shader->GetVertexType() == vertexType) {
			return shader;
		}
	}

	ShaderConfig config;
	config.m_name = shaderName;
	config.m_vertexType = vertexType;
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

	D3D11_INPUT_ELEMENT_DESC const* inputElementDesc = nullptr;
	UINT numElements = 0;
	if (vertexType == VertexType::VERTEX_PCUTBN) {
		static D3D11_INPUT_ELEMENT_DESC const pcutbnInputElementDesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		inputElementDesc = pcutbnInputElementDesc;
		numElements = ARRAYSIZE(pcutbnInputElementDesc);
	}
	else {
		static D3D11_INPUT_ELEMENT_DESC const pcuInputElementDesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		inputElementDesc = pcuInputElementDesc;
		numElements = ARRAYSIZE(pcuInputElementDesc);
	}

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
Shader* Renderer::CreateShader(char const* shaderName, VertexType vertexType)
{
	std::string filename = std::string("Data/Shaders/") + shaderName + ".hlsl";
	std::string shaderSource;
	int bytesRead = FileReadToString(shaderSource, filename);
	if (bytesRead == 0)
	{
		ERROR_AND_DIE(Stringf("Could not read shader file: %s", filename.c_str()));
	}
	return CreateShader(shaderName, shaderSource.c_str(), vertexType);
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
	if (shader == nullptr)
	{
		shader = m_defaultShader;
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
	Image image(imageFilePath);
	Texture* newTexture = CreateTextureFromImage(image);
	return newTexture;
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromImage(const Image& image)
{
	Texture* newTexture = new Texture();
	newTexture->m_name = image.GetImageFilePath();
	newTexture->m_dimensions = image.GetDimensions();

	D3D11_TEXTURE2D_DESC textureDesc = { };
	textureDesc.Width = image.GetDimensions().x;
	textureDesc.Height = image.GetDimensions().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureData;
	textureData.pSysMem = image.GetRawData();
	textureData.SysMemPitch = 4 * image.GetDimensions().x;

	HRESULT hr;
	hr = m_device->CreateTexture2D(&textureDesc, &textureData, &newTexture->m_texture);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateTextureFromImage failed for image file \"%s\".",
			image.GetImageFilePath().c_str()));
	}

	hr = m_device->CreateShaderResourceView(newTexture->m_texture, NULL,
		&newTexture->m_shaderResourceView);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateShaderResourceView failed for image file \"%s\".",
			image.GetImageFilePath().c_str()));
	}

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
void Renderer::BindTexture(const Texture* texture)
{
	if (texture == nullptr)
	{
		texture = m_defaultTexture;
	}

	m_currentTexture = texture;
	m_deviceContext->PSSetShaderResources(0, 1, &m_currentTexture->m_shaderResourceView);
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetBlendMode(BlendMode blendMode)
{
	m_desiredBlendMode = blendMode;
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetSamplerMode(SamplerMode samplerMode)
{
	m_desiredSamplerMode = samplerMode;
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetDepthMode(DepthMode depthMode)
{
	m_desiredDepthMode = depthMode;
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetRasterizerMode(RasterizerMode rasterizerMode)
{
	m_desiredRasterizerMode = rasterizerMode;
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetStatesIfChanged()
{
	if (m_rasterizerState != m_rasterizerStates[(int)m_desiredRasterizerMode])
	{
		m_rasterizerState = m_rasterizerStates[(int)m_desiredRasterizerMode];
		m_deviceContext->RSSetState(m_rasterizerState);
	}

	if (m_blendState != m_blendStates[(int)m_desiredBlendMode])
	{
		m_blendState = m_blendStates[(int)m_desiredBlendMode];
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		UINT sampleMask = 0xffffffff;
		m_deviceContext->OMSetBlendState(m_blendState, blendFactor, sampleMask);
	}

	if (m_samplerState != m_samplerStates[(int)m_desiredSamplerMode])
	{
		m_samplerState = m_samplerStates[(int)m_desiredSamplerMode];
		m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	}

	if (m_depthStencilState != m_depthStencilStates[(int)m_desiredDepthMode])
	{
		m_depthStencilState = m_depthStencilStates[(int)m_desiredDepthMode];
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
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