#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"

extern bool						g_debugDraw;
extern RandomNumberGenerator	g_appRNG;

Game::Game()
{
	// Initialize attract camera view
	m_attractCamera = new Camera();
	m_attractCamera->SetOrthoView(Vec2(0, 0), Vec2(ATTRACT_SCREEN_SIZE_X, ATTRACT_SCREEN_SIZE_Y));

	// Initialize world camera view
	m_worldCamera = new Camera();
	m_worldCamera->SetOrthoView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));

	// Initialize screen camera view
	m_screenCamera = new Camera();
	m_screenCamera->SetOrthoView(Vec2(0, 0), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
}

Game::~Game()
{
	if (m_attractCamera != nullptr) {
		delete m_attractCamera;
	}
	if (m_worldCamera != nullptr) {
		delete m_worldCamera;
	}
	if (m_screenCamera != nullptr) {
		delete m_screenCamera;
	}
}

void Game::Update()
{
	UpdateEntities();

	UpdateFromKeyboard();
	UpdateFromController();

	DeleteGarbageEntities();
}

void Game::Render() const
{
	switch (m_currentGameState)
	{
	case GameStates::ATTRACT:	Render_Attract();	break;
	case GameStates::PLAYING:	Render_Playing();	break;
		break;
	default:
		break;
	}
}

void Game::UpdateFromKeyboard()
{
	// Handle ESC
	if (g_engine->m_input->WasKeyJustPressed(KEYCODE_ESC)) {
		m_nextGameState = GameStates::ATTRACT;
	}
}

void Game::UpdateFromController()
{
// 	if ((g_engine->m_input->GetController(m_playerShip->m_xboxControllerID).WasButtonJustPressed(XboxButtonID::START)) && 
// 		(!m_playerShip->IsAlive()) && 
// 		m_playerShip->m_extraLives > 0) {
// 		RespawnPlayer();
// 	}
}

void Game::UpdateEntities()
{
// 	if (m_playerShip->IsAlive()) { m_playerShip->Update(deltaSeconds); }
// 
// 	UpdateEntityList(MAX_ASTEROIDS, m_asteroids, deltaSeconds);
// 	UpdateEntityList(MAX_BULLETS, m_bullets, deltaSeconds);
}

void Game::DeleteGarbageEntities()
{
// 	if (m_playerShip->m_isGarbage) { delete m_playerShip; }
// 
// 	DeleteGarbageEntitiesInList(MAX_ASTEROIDS, m_asteroids);
// 	DeleteGarbageEntitiesInList(MAX_BULLETS, m_bullets);
}

void Game::Render_Attract() const
{
	g_engine->m_render->ClearScreen(Rgba8(64, 80, 96, 255));
	g_engine->m_render->BeginCamera(*m_attractCamera);

	// Render test
	float ringOuterRadius = 16.f + abs(static_cast<float>(static_cast<int>(timeCurrentFrame * 100) % 100) * 0.1f - 5.f);
	DrawFadedRing(Vec2(25.f, 50.f), 8.f, ringOuterRadius, Rgba8(232, 232, 96, 255), Rgba8(232, 232, 96, 255));
	DebugDrawLine(Vec2(42.5f, 50.f), Vec2(1575.f, 50.f), 4.f, Rgba8(232, 232, 0, 255), Rgba8(0, 232, 232, 255));

	// Render texture
	Texture* testTexture = g_engine->m_render->CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");
	std::vector<Vertex> testTextureVerts(6);
	testTextureVerts[0] = Vertex(Vec3(600.f, 200.f, 0.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f));
	testTextureVerts[1] = Vertex(Vec3(600.f, 600.f, 0.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 1.f));
	testTextureVerts[2] = Vertex(Vec3(1000.f, 200.f, 0.f), Rgba8(255, 255, 255, 255), Vec2(1.f, 0.f));
	testTextureVerts[3] = Vertex(Vec3(600.f, 600.f, 0.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 1.f));
	testTextureVerts[4] = Vertex(Vec3(1000.f, 200.f, 0.f), Rgba8(255, 255, 255, 255), Vec2(1.f, 0.f));
	testTextureVerts[5] = Vertex(Vec3(1000.f, 600.f, 0.f), Rgba8(255, 255, 255, 255), Vec2(1.f, 1.f));
	g_engine->m_render->BindTexture(testTexture);
	g_engine->m_render->DrawVertexArray(testTextureVerts);
	g_engine->m_render->BindTexture(nullptr);

	// Render text
	std::vector<Vertex> textVerts;
	AddVertsForTextTriangles2D(textVerts, "GAMESTATE_ATTRACT", Vec2(50.f, 50.f), 50.f, Rgba8(232, 232, 232, 255));
	g_engine->m_render->DrawVertexArray(static_cast<int>(textVerts.size()), textVerts.data());

	g_engine->m_render->EndCamera(*m_attractCamera);
}

void Game::Render_Playing() const
{
	// TODO: Render game world
	g_engine->m_render->BeginCamera(*m_worldCamera);
	{
		std::vector<Vertex> textVerts;
		AddVertsForTextTriangles2D(textVerts, "GAMESTATE_PLAY", Vec2(0.f, 0.f), 10.f, Rgba8(144, 196, 232, 255));
		g_engine->m_render->DrawVertexArray(static_cast<int>(textVerts.size()), textVerts.data());
	}
	g_engine->m_render->EndCamera(*m_worldCamera);

	// TODO: Render ui screen
	g_engine->m_render->BeginCamera(*m_screenCamera);
	if (m_gameClock.IsPaused()) {
		std::vector<Vertex> pausedBgVerts(6);
		pausedBgVerts[0] = Vertex(Vec3(0.f, 0.f, 0.f), Rgba8(64, 64, 64, 64), Vec2(0.f, 0.f));
		pausedBgVerts[1] = Vertex(Vec3(0.f, SCREEN_SIZE_Y, 0.f), Rgba8(64, 64, 64, 64), Vec2(0.f, 0.f));
		pausedBgVerts[2] = Vertex(Vec3(SCREEN_SIZE_X, 0.f, 0.f), Rgba8(64, 64, 64, 64), Vec2(0.f, 0.f));
		pausedBgVerts[3] = Vertex(Vec3(0.f, SCREEN_SIZE_Y, 0.f), Rgba8(64, 64, 64, 64), Vec2(0.f, 0.f));
		pausedBgVerts[4] = Vertex(Vec3(SCREEN_SIZE_X, 0.f, 0.f), Rgba8(64, 64, 64, 64), Vec2(0.f, 0.f));
		pausedBgVerts[5] = Vertex(Vec3(SCREEN_SIZE_X, SCREEN_SIZE_Y, 0.f), Rgba8(64, 64, 64, 64), Vec2(0.f, 0.f));
		g_engine->m_render->DrawVertexArray(pausedBgVerts);
	}
	g_engine->m_render->EndCamera(*m_screenCamera);
}

float Game::GetDeltaSeconds() const
{
	return static_cast<float>(m_gameClock.GetDeltaSeconds());
}



