#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/Window.hpp"

//-----------------------------------------------------------------------------------------------
class Engine;
class Window;
class Renderer;
class InputSystem;
class AudioSystem;

//-----------------------------------------------------------------------------------------------
struct EngineConfig {
	AudioConfig		m_audioConfig;
	InputConfig		m_inputConfig;
	RenderConfig	m_renderConfig;
	WindowConfig	m_windowConfig;
};

//-----------------------------------------------------------------------------------------------
extern Engine* g_engine; // Advertisement that this global exists, so external people can use it

//-----------------------------------------------------------------------------------------------
class Engine
{
public:
	Engine(EngineConfig const& config);
	~Engine();
	void BeginFrame();
	void EndFrame();

public:
	Window*			m_window	= nullptr;
	Renderer*		m_render	= nullptr;
	InputSystem*	m_input		= nullptr;
	AudioSystem*	m_audio		= nullptr;

	EngineConfig	m_config;
};