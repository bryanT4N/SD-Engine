#pragma once

//-----------------------------------------------------------------------------------------------
class Engine;
class Window;
class Renderer;
class InputSystem;
class AudioSystem;

//-----------------------------------------------------------------------------------------------
extern Engine* g_engine; // Advertisement that this global exists, so external people can use it

//-----------------------------------------------------------------------------------------------
class Engine
{
public:
	Engine();
	~Engine();
	void BeginFrame();
	void EndFrame();

public:
	Window*			m_window	= nullptr;
	Renderer*		m_render	= nullptr;
	InputSystem*	m_input		= nullptr;
	AudioSystem*	m_audio		= nullptr;
};