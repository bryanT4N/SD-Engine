#pragma once
#include <string>

//-----------------------------------------------------------------------------------------------
struct WindowConfig {
	bool				m_isEnabled = true;
	float				m_clientAspect = (16.f / 9.f);
	std::string			m_windowTitle = "Unnamed SD Application";
};

//-----------------------------------------------------------------------------------------------
struct Vec2;

//-----------------------------------------------------------------------------------------------
class Window
{
public:
	Window(WindowConfig const& config);
	~Window();
	void Startup(); 
	void Shutdown() const;
	void BeginFrame();
	void EndFrame() const;

	Vec2 GetNormalizedMouseUV() const;

	WindowConfig		m_config;
	void*				m_displayDeviceContext = 0;
	void*				m_windowHandle;

private:
	void CreateOSWindow();
	void RunMessagePump();
};