#pragma once
#include <string>

//-----------------------------------------------------------------------------------------------
struct WindowConfig {
	bool				m_isEnabled = true;
	float				m_clientAspect = (16.f / 9.f);
	std::string			m_windowTitle = "Unnamed SD Application";
};

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

	WindowConfig		m_config;
	void*				m_displayDeviceContext = 0;

private:
	void CreateOSWindow();
	void RunMessagePump();
};