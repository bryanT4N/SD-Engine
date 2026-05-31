#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)
#include <math.h>
#include <cassert>
#include <crtdbg.h>

#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"

//-----------------------------------------------------------------------------------------------
// Later we will move this useful macro to a more central place, e.g. Engine/Core/EngineCommon.hpp
#define UNUSED(x) (void)(x);

HDC g_displayDeviceContext = nullptr;				// ...becomes void* Window::m_displayContext



//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( applicationInstanceHandle );
	UNUSED( commandLineString );

//	CreateOSWindow( applicationInstanceHandle, WORLD_SIZE_X / WORLD_SIZE_Y );	

	g_theApp = new App();

	//Program main loop; keep running frames until it's time to quit
	while (!g_theApp->IsQuitting())
	{
		g_theApp->RunFrame();
	}
	
	delete g_theApp;
	g_theApp = nullptr;

	return 0;
	
}


