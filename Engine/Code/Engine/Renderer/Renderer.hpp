#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>

//-----------------------------------------------------------------------------------------------
struct Rgba8;
struct Vertex;
class Camera;
class Texture;

//-----------------------------------------------------------------------------------------------
struct RenderConfig {
	bool m_isEnabled = true;
};

//-----------------------------------------------------------------------------------------------
class Renderer
{
public:
	RenderConfig m_config;
	std::vector<Texture*> m_loadedTextures;

public:
	Renderer(RenderConfig const& config);
	~Renderer();
	void Startup() const; 
	void Shutdown() const;
	void BeginFrame() const;
	void EndFrame() const;

	void CreateRenderingContext() const;
	void ClearScreen(Rgba8 const& clearColor) const;

	void BeginCamera(Camera const& camera) const;
	void EndCamera(Camera const& camera) const;

	void DrawVertexArray(int numVertexes, Vertex const* vertexes) const;
	void DrawVertexArray(const std::vector<Vertex>& vertexes) const;

	void BindTexture(Texture* texture);
	Texture* CreateOrGetTextureFromFile(char const* imageFilePath);
	Texture* CreateTextureFromFile(char const* imageFilePath);
	Texture* CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData);
	Texture* GetTextureFromFileName(char const* imageFilePath);
};