#include "Engine/Core/Engine.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

Engine* g_engine = nullptr; 

//-----------------------------------------------------------------------------------------------
Engine::Engine(EngineConfig const& config)
	: m_config( config )
{
	// First pass: create all engine subsystems, so all pointers/objects exist;
	// Second pass is actual startup, now that all system objects exist.

	// Make this Engine instance globally visible as early as possible
	g_engine = this;

	// Create engine subsystems, if requested by Game/App
	if (m_config.m_windowConfig.m_isEnabled)			m_window		= new Window(m_config.m_windowConfig);
	if (m_config.m_renderConfig.m_isEnabled)			m_render		= new Renderer( m_config.m_renderConfig );
	if (m_config.m_inputConfig.m_isEnabled)				m_input			= new InputSystem( m_config.m_inputConfig );
	if (m_config.m_audioConfig.m_isEnabled)				m_audio			= new AudioSystem( m_config.m_audioConfig );
	if (m_config.m_eventSystemConfig.m_isEnabled)		m_eventSystem	= new EventSystem( m_config.m_eventSystemConfig );
	if (m_config.m_devConsoleConfig.m_isEnabled)		m_devConsole	= new DevConsole( m_config.m_devConsoleConfig );

	// Start up existing engine subsystems
	if(m_window)		m_window->Startup();
	if(m_render)		m_render->Startup();
	if(m_input)			m_input->Startup();
	if(m_audio)			m_audio->Startup();
	if(m_eventSystem)	m_eventSystem->Startup();
	if(m_devConsole)	m_devConsole->Startup();
}

//-----------------------------------------------------------------------------------------------
Engine::~Engine()
{
	if( m_devConsole != nullptr )
	{
		m_devConsole->Shutdown();
	}
	if( m_eventSystem != nullptr )
	{
		m_eventSystem->Shutdown();
	}

	m_render->Shutdown();
	m_input->Shutdown();
	m_audio->Shutdown();
	m_window->Shutdown();

	delete m_devConsole;
	m_devConsole = nullptr;

	delete m_eventSystem;
	m_eventSystem = nullptr;

	delete m_input;
	m_input = nullptr;

	delete m_audio;
	m_audio = nullptr; 

	delete m_render;
	m_render = nullptr;

	delete m_window;
	m_window = nullptr;

	g_engine = nullptr;
}

//-----------------------------------------------------------------------------------------------
void Engine::BeginFrame()
{
	m_window->BeginFrame();
	m_render->BeginFrame();
	m_input->BeginFrame();
	m_audio->BeginFrame();

	if( m_eventSystem != nullptr )
	{
		m_eventSystem->BeginFrame();
	}
	if( m_devConsole != nullptr )
	{
		m_devConsole->BeginFrame();
	}
}

//-----------------------------------------------------------------------------------------------
void Engine::EndFrame()
{
	if( m_eventSystem != nullptr )
	{
		m_eventSystem->EndFrame();
	}
	if( m_devConsole != nullptr )
	{
		m_devConsole->EndFrame();
	}

	m_window->EndFrame();
	m_render->EndFrame();
	m_input->EndFrame();
	m_audio->EndFrame();
}

//-----------------------------------------------------------------------------------------------

