#pragma once


//-----------------------------------------------------------------------------------------------
class Window
{
public:
	Window();
	~Window();
	void Startup() const; 
	void Shutdown() const;
	void BeginFrame() const;
	void EndFrame() const;
};