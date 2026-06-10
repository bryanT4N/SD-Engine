#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/UI/UITheme.hpp"
#include "Engine/Window/Window.hpp"

App*					g_theApp		= nullptr;
bool					g_debugDraw		= false;
RandomNumberGenerator	g_appRNG;

double					timeCurrentFrame;
double					timePreviousFrame;

static void PrintStartupControlsToDevConsole()
{
	if (g_engine == nullptr || g_engine->m_devConsole == nullptr) {
		return;
	}

	DevConsole* devConsole = g_engine->m_devConsole;
	devConsole->AddLine(DevConsole::LOG_COLOR_INFO_MAJOR, "AceAttorneyApproximation Controls:");
	std::string controlsBlockText =
		"Mouse  - Aim\n"
		"W / A  - Move\n"
		"S / D  - Strafe\n"
		"Q / E  - Roll\n"
		"Z / C  - Elevate\n"
		"Shift  - Sprint\n"
		"H      - Set Camera to Origin\n"
		"1      - Spawn Line\n"
		"2      - Spawn Point\n"
		"3      - Spawn Wireframe Sphere\n"
		"4      - Spawn Basis\n"
		"5      - Spawn Billboarded Text\n"
		"6      - Spawn Wireframe Cylinder\n"
		"7      - Add Message\n"
		"`~`    - Open Dev Console\n"
		"Escape - Exit Game\n"
		"Space  - Start Game";
	devConsole->AddLine(DevConsole::LOG_COLOR_INFO_MINOR, controlsBlockText);
}

App::App()
{
	XmlDocument gameConfigXml;
	XmlResult gameConfigResult = gameConfigXml.LoadFile("Data/GameConfig.xml");
	if (gameConfigResult == tinyxml2::XML_SUCCESS) {
		XmlElement* root = gameConfigXml.RootElement();
		if (root != nullptr) {
			g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*root);
		}
	}

	EngineConfig		config;
	config.m_windowConfig.m_clientAspect = g_gameConfigBlackboard.GetValue("windowAspect", 2.0f);
	config.m_windowConfig.m_windowTitle =
		g_gameConfigBlackboard.GetValue("windowTitle", std::string("AceAttorneyApproximation"));
	config.m_devConsoleConfig.m_fontName = "Data/Fonts/SquirrelFixedFont";
	config.m_devConsoleConfig.m_fontAspect = 0.7f;

	g_engine			= new Engine(config);
	DebugRenderConfig debugRenderConfig;
	debugRenderConfig.m_renderer = g_engine->m_render;
	DebugRenderSystemStartup(debugRenderConfig);

	g_uiTheme = new UITheme();
	g_uiTheme->defaultFont = g_engine->m_render->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");

	m_game				= new Game;
	SubscribeEventCallbackFunction("Quit", App::Command_Quit);
	PrintStartupControlsToDevConsole();

	timeCurrentFrame	= GetCurrentTimeSeconds();
	timePreviousFrame	= timeCurrentFrame;
}

App::~App()
{
	UnsubscribeEventCallbackFunction("Quit", App::Command_Quit);
	delete m_game;
	delete g_uiTheme;
	g_uiTheme = nullptr;
	DebugRenderSystemShutdown();
	delete g_engine;
}

void App::RunFrame()
{
	timeCurrentFrame = GetCurrentTimeSeconds();

	Clock::TickSystemClock();
	UpdateCursorMode();

	// Engine systems only, do something before Update
	g_engine->BeginFrame();
	DebugRenderBeginFrame();

	UpdateFromKeyboard();

	// Update game if m_currentGameState == GAMESTATE_PLAY
	if (m_game->m_currentGameState == GameStates::PLAYING) {
		m_game->Update();
	}

	// Render game
	g_engine->m_render->ClearScreen(Rgba8(64, 80, 96, 255));
	m_game->Render();

	if (g_engine->m_devConsole != nullptr && m_game->m_screenCamera != nullptr) {
		g_engine->m_render->BeginCamera(*m_game->m_screenCamera);
		g_engine->m_devConsole->Render(AABB2(Vec2(0.0f, 0.0f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y)));
		g_engine->m_render->EndCamera(*m_game->m_screenCamera);
	}

	// Engine systems only, do something after Render
	DebugRenderEndFrame();
	g_engine->EndFrame();

	m_game->m_currentGameState = m_game->m_nextGameState;
	timePreviousFrame = timeCurrentFrame;
}

void App::UpdateCursorMode()
{
	if (g_engine == nullptr || g_engine->m_input == nullptr) {
		return;
	}

	bool windowHasFocus = true;
	if (g_engine->m_window != nullptr) {
		windowHasFocus = g_engine->m_window->HasFocus();
	}

	bool isDevConsoleOpen = false;
	if (g_engine->m_devConsole != nullptr) {
		isDevConsoleOpen = g_engine->m_devConsole->IsOpen();
	}

	bool isAttractState = true;
	if (m_game != nullptr) {
		isAttractState = (m_game->m_currentGameState == GameStates::ATTRACT);
	}

	if (!windowHasFocus || isDevConsoleOpen || isAttractState) {
		g_engine->m_input->SetCursorMode(CursorMode::POINTER);
	}
	else {
		g_engine->m_input->SetCursorMode(CursorMode::FPS);
	}
}

void App::SetIsQuitting()
{
	m_isQuitting = true;
}

bool App::IsQuitting() const
{
	return m_isQuitting;
}

void App::UpdateFromKeyboard()
{
	if (m_game->m_currentGameState == GameStates::ATTRACT) {
		if (g_engine->m_input->WasKeyJustPressed(KEYCODE_ESC)) {
			SetIsQuitting();
		}
		if (g_engine->m_input->WasKeyJustPressed('N') || 
			g_engine->m_input->WasKeyJustPressed(KEYCODE_SPACE) || 
			g_engine->m_input->GetController(0).IsButtonDown(XboxButtonID::START) || 
			g_engine->m_input->GetController(0).IsButtonDown(XboxButtonID::A)){
			delete m_game;
			m_game = new Game();
			m_game->m_nextGameState = GameStates::PLAYING;
		}
	}
	else {
		// Pause
		if (g_engine->m_input->WasKeyJustPressed('P')) {
			m_game->m_gameClock.TogglePause();
		}

		// Pause after next update
		if (g_engine->m_input->WasKeyJustPressed('O')) {
			m_game->m_gameClock.StepSingleFrame();
		}

		// Slow mode while holding T
		if (!m_game->m_gameClock.IsPaused()) {
			if (g_engine->m_input->IsKeyDown('T')) {
				float slowMoTimeScale = g_gameConfigBlackboard.GetValue("gameplaySlowMoTimeScale", 0.1f);
				m_game->m_gameClock.SetTimeScale(slowMoTimeScale);
			}
			else {
				m_game->m_gameClock.SetTimeScale(1.0);
			}
		}

		// Debug draw
		if (g_engine->m_input->WasKeyJustPressed(KEYCODE_F1)) {
			g_debugDraw = !g_debugDraw;
		}

		// Restart game
		if (g_engine->m_input->WasKeyJustPressed(KEYCODE_F8)) {
			delete m_game;
			m_game = new Game();
		}
	}
}

//-----------------------------------------------------------------------------------------------
bool App::Command_Quit([[maybe_unused]]EventArgs& args)
{
	if (g_theApp != nullptr) {
		g_theApp->SetIsQuitting();
	}
	return true;
}
