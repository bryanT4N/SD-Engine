#pragma once

//-----------------------------------------------------------------------------------------------
struct WindowConfig {
	bool m_isEnabled = true;
};

//-----------------------------------------------------------------------------------------------
class Window
{
public:
	Window(WindowConfig const& config);
	~Window();
	void Startup() const; 
	void Shutdown() const;
	void BeginFrame() const;
	void EndFrame() const;

	WindowConfig m_config;
};