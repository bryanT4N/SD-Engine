#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/DevConsole.hpp"

//-----------------------------------------------------------------------------------------------
class Engine;
class Window;
class Renderer;
class InputSystem;
class AudioSystem;
class EventSystem;
class DevConsole;

//-----------------------------------------------------------------------------------------------
struct EngineConfig {
	AudioConfig			m_audioConfig;
	InputConfig			m_inputConfig;
	RenderConfig		m_renderConfig;
	WindowConfig		m_windowConfig;
	EventSystemConfig	m_eventSystemConfig;
	DevConsoleConfig	m_devConsoleConfig;
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
	Window*			m_window		= nullptr;
	Renderer*		m_render		= nullptr;
	InputSystem*	m_input			= nullptr;
	AudioSystem*	m_audio			= nullptr;
	EventSystem*	m_eventSystem	= nullptr;
	DevConsole*		m_devConsole	= nullptr;

	EngineConfig	m_config;
};