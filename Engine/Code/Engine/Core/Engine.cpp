#include "Engine/Core/Engine.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

Engine* g_engine = nullptr; 

//-----------------------------------------------------------------------------------------------
Engine::Engine(EngineConfig const& config)
{
	m_window	= new Window(config.m_windowConfig);
	m_render	= new Renderer(config.m_renderConfig);
	m_input		= new InputSystem(config.m_inputConfig);
	m_audio		= new AudioSystem(config.m_audioConfig);

	m_window->Startup();
	m_render->Startup();
	m_input->Startup();
	m_audio->Startup();
}

//-----------------------------------------------------------------------------------------------
Engine::~Engine()
{
	m_render->Shutdown();
	m_input->Shutdown();
	m_audio->Shutdown();
	m_window->Shutdown();

	delete m_input;
	m_input = nullptr;

	delete m_audio;
	m_audio = nullptr; 

	delete m_render;
	m_render = nullptr;

	delete m_window;
	m_window = nullptr;
}

//-----------------------------------------------------------------------------------------------
void Engine::BeginFrame()
{
	m_window->BeginFrame();
	m_render->BeginFrame();
	m_input->BeginFrame();
	m_audio->BeginFrame();
}

//-----------------------------------------------------------------------------------------------
void Engine::EndFrame()
{
	m_window->EndFrame();
	m_render->EndFrame();
	m_input->EndFrame();
	m_audio->EndFrame();
}

//-----------------------------------------------------------------------------------------------

