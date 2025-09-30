#pragma once

//-----------------------------------------------------------------------------------------------
class Engine;		
class Renderer;
class InputSystem;

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
	Renderer*		m_render	= nullptr;
	InputSystem*	m_input		= nullptr;
};