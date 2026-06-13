#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Game/DialogueScreen.hpp"
#include "Engine/Narrative/DialogueDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Math/Vec2.hpp"


static float ComputeAttractRingOuterRadius()
{
	double timeSeconds = GetCurrentTimeSeconds();
	int pulseTicks = static_cast<int>(timeSeconds * ATTRACT_RING_PULSE_TIME_SCALE) %
		ATTRACT_RING_PULSE_PERIOD_TICKS;
	float pulseValue = static_cast<float>(pulseTicks) * ATTRACT_RING_PULSE_SCALE;
	return ATTRACT_RING_BASE_OUTER_RADIUS + abs(pulseValue - ATTRACT_RING_HALF_PULSE_SPAN);
}

Game::Game()
{
	// Initialize attract camera view
	m_attractCamera = new Camera();
	m_attractCamera->SetOrthoView(Vec2(0, 0), Vec2(ATTRACT_SCREEN_SIZE_X, ATTRACT_SCREEN_SIZE_Y));

	// Initialize screen camera view
	m_screenCamera = new Camera();
	m_screenCamera->SetOrthoView(Vec2(0, 0), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));

	Startup();
}

Game::~Game()
{
	Shutdown();

	if (m_attractCamera != nullptr) {
		delete m_attractCamera;
		m_attractCamera = nullptr;
	}
	if (m_screenCamera != nullptr) {
		delete m_screenCamera;
		m_screenCamera = nullptr;
	}
}

void Game::Startup()
{
	m_player = new Player(this);
	AddEntity(m_player);

	m_dialogueDatabase.LoadFile("Data/Dialogues/test.xml");
	m_dialogueScreen = new DialogueScreen();
	m_dialogueScreen->Build(g_uiTheme);
}

void Game::Shutdown()
{
	delete m_dialogueScreen;
	m_dialogueScreen = nullptr;

	const int entityCount = static_cast<int>(m_entities.size());
	for (int entityIndex = 0; entityIndex < entityCount; ++entityIndex) {
		delete m_entities[entityIndex];
		m_entities[entityIndex] = nullptr;
	}

	m_entities.clear();
	m_player = nullptr;
}

void Game::AddEntity(Entity* entity)
{
	if (entity == nullptr) {
		return;
	}

	m_entities.push_back(entity);
}

void Game::Update()
{
	UpdateEntities();

	UpdateFromKeyboard();
	UpdateFromController();

	if (m_dialogueScreen != nullptr && m_dialogueScreen->IsActive()) {
		m_dialogueScreen->Update(GetDeltaSeconds());
	}

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
	if (g_engine == nullptr || g_engine->m_input == nullptr) {
		return;
	}

	InputSystem* inputSystem = g_engine->m_input;

	// Handle ESC
	if (inputSystem->WasKeyJustPressed(KEYCODE_ESC)) {
		m_nextGameState = GameStates::ATTRACT;
	}

	if (inputSystem->WasKeyJustPressed(KEYCODE_SPACE) &&
		m_dialogueScreen != nullptr && !m_dialogueScreen->IsActive()) {
		DialogueDefinition const* dialogue = m_dialogueDatabase.Find("test_dialogue");
		m_dialogueScreen->Open(dialogue, &m_worldState);
	}
}

void Game::UpdateFromController()
{
	// Reserved for game-state-level controller actions.
}

void Game::UpdateEntities()
{
	float const deltaSeconds = GetDeltaSeconds();
	const int entityCount = static_cast<int>(m_entities.size());
	for (int entityIndex = 0; entityIndex < entityCount; ++entityIndex) {
		Entity* entity = m_entities[entityIndex];
		if (entity != nullptr) {
			entity->Update(deltaSeconds);
		}
	}
}

void Game::DeleteGarbageEntities()
{
}

void Game::Render_Attract() const
{
	g_engine->m_render->ClearScreen(Rgba8(64, 80, 96, 255));
	g_engine->m_render->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_engine->m_render->SetDepthMode(DepthMode::DISABLED);
	g_engine->m_render->BeginCamera(*m_attractCamera);

	// Render test
	float ringOuterRadius = ComputeAttractRingOuterRadius();
	DrawFadedRing(
		Vec2(25.f, 50.f),
		8.f,
		ringOuterRadius,
		Rgba8(232, 232, 96, 255),
		Rgba8(232, 232, 96, 255));
	DebugDrawLine(
		Vec2(42.5f, 50.f),
		Vec2(1575.f, 50.f),
		4.f,
		Rgba8(232, 232, 0, 255),
		Rgba8(0, 232, 232, 255));

	// Render texture
	Texture* testTexture =
		g_engine->m_render->CreateOrGetTextureFromFile(
			"Data/Images/Test_StbiFlippedAndOpenGL.png");
	std::vector<Vertex> testTextureVerts;
	AddVertsForAABB2D(
		testTextureVerts,
		AABB2(600.f, 200.f, 1000.f, 600.f),
		Rgba8::WHITE,
		AABB2(0.f, 0.f, 1.f, 1.f));
	g_engine->m_render->BindTexture(testTexture);
	g_engine->m_render->DrawVertexArray(testTextureVerts);
	g_engine->m_render->BindTexture(nullptr);

	// Render text
	std::vector<Vertex> textVerts;
	AddVertsForTextTriangles2D(
		textVerts,
		"GAMESTATE_ATTRACT",
		Vec2(50.f, 50.f),
		50.f,
		Rgba8(232, 232, 232, 255));
	g_engine->m_render->DrawVertexArray(static_cast<int>(textVerts.size()), textVerts.data());

	g_engine->m_render->EndCamera(*m_attractCamera);
}

void Game::Render_Playing() const
{
	if (m_player == nullptr) {
		return;
	}

	Camera const& worldCamera = m_player->GetCamera();
	g_engine->m_render->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_engine->m_render->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_engine->m_render->BeginCamera(worldCamera);
	{
		const int entityCount = static_cast<int>(m_entities.size());
		for (int entityIndex = 0; entityIndex < entityCount; ++entityIndex) {
			Entity const* entity = m_entities[entityIndex];
			if (entity != nullptr) {
				g_engine->m_render->SetModelCBO(
					entity->GetModelToWorldTransform(),
					entity->m_color);
				entity->Render();
			}
		}
		DebugRenderWorld(worldCamera);
	}
	g_engine->m_render->EndCamera(worldCamera);

	// TODO: Render ui screen
	g_engine->m_render->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_engine->m_render->SetDepthMode(DepthMode::DISABLED);
	g_engine->m_render->BeginCamera(*m_screenCamera);

	double totalGameSeconds = m_gameClock.GetTotalSeconds();
	double frameRate = Clock::GetSystemClock().GetFrameRate();
	double timeScale = m_gameClock.GetTimeScale();
	std::string topRightText = Stringf("Time: %.2f FPS: %.1f Scale: %.2f", totalGameSeconds, frameRate, timeScale);
	DebugAddScreenText(
		topRightText,
		AABB2(SCREEN_SIZE_X - 520.f, SCREEN_SIZE_Y - 30.f, SCREEN_SIZE_X - 10.f, SCREEN_SIZE_Y - 5.f),
		10.f,
		Vec2(1.f, 1.f),
		0.f,
		Rgba8::WHITE,
		Rgba8::WHITE);
	bool isDialogueActive = (m_dialogueScreen != nullptr && m_dialogueScreen->IsActive());
	if (!isDialogueActive) {
		DebugAddScreenText(
			"Press Space to start the test dialogue",
			AABB2(0.f, 0.f, SCREEN_SIZE_X, SCREEN_SIZE_Y),
			20.f,
			Vec2(0.5f, 0.5f),
			0.f,
			Rgba8::WHITE,
			Rgba8::WHITE);
	}

	if (m_gameClock.IsPaused()) {
		std::vector<Vertex> pausedBgVerts;
		AddVertsForAABB2D(
			pausedBgVerts,
			AABB2(0.f, 0.f, SCREEN_SIZE_X, SCREEN_SIZE_Y),
			Rgba8(64, 64, 64, 64),
			AABB2(0.f, 0.f, 0.f, 0.f));
		g_engine->m_render->DrawVertexArray(pausedBgVerts);
	}
	DebugRenderScreen(*m_screenCamera);

	if (isDialogueActive) {
		m_dialogueScreen->Render(*g_engine->m_render);
	}

	g_engine->m_render->EndCamera(*m_screenCamera);
}

float Game::GetDeltaSeconds() const
{
	return static_cast<float>(m_gameClock.GetDeltaSeconds());
}



