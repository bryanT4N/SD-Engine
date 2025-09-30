#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"

Engine* g_engine = nullptr; 

//-----------------------------------------------------------------------------------------------
Engine::Engine()
{
	m_render = new Renderer();
	m_input = new InputSystem();

	m_render->Startup();
	m_input->Startup();
}

//-----------------------------------------------------------------------------------------------
Engine::~Engine()
{
	m_render->Shutdown();
	m_input->Shutdown();

	delete m_render;
	m_render = nullptr;
	delete m_input;
	m_input = nullptr;
}

//-----------------------------------------------------------------------------------------------
void Engine::BeginFrame()
{
	m_render->BeginFrame();
	m_input->BeginFrame();
}

//-----------------------------------------------------------------------------------------------
void Engine::EndFrame()
{
	m_render->EndFrame();
	m_input->EndFrame();
}

//-----------------------------------------------------------------------------------------------

