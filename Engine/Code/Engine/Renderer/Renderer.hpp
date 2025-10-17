#pragma once


//-----------------------------------------------------------------------------------------------
struct Rgba8;
struct Vertex;
class Camera;

//-----------------------------------------------------------------------------------------------
struct RenderConfig {
	bool m_isEnabled = true;
};

//-----------------------------------------------------------------------------------------------
class Renderer
{
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

	RenderConfig m_config;
};