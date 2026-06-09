#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
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


static Texture* GetTexture(char const* texturePath)
{
	if (g_engine == nullptr || g_engine->m_render == nullptr) {
		return nullptr;
	}

	return g_engine->m_render->CreateOrGetTextureFromFile(texturePath);
}

static float ComputeAttractRingOuterRadius()
{
	double timeSeconds = GetCurrentTimeSeconds();
	int pulseTicks = static_cast<int>(timeSeconds * ATTRACT_RING_PULSE_TIME_SCALE) %
		ATTRACT_RING_PULSE_PERIOD_TICKS;
	float pulseValue = static_cast<float>(pulseTicks) * ATTRACT_RING_PULSE_SCALE;
	return ATTRACT_RING_BASE_OUTER_RADIUS + abs(pulseValue - ATTRACT_RING_HALF_PULSE_SPAN);
}

static Vec3 GetPlayerForwardDirection(Player const& player)
{
	Camera const& playerCamera = player.GetCamera();
	EulerAngles const cameraOrientation = playerCamera.GetOrientation();
	return cameraOrientation.GetForwardDir_IFwd_JLeft_KUp();
}

static std::string BuildPlayerPoseDebugText(Player const& player)
{
	Vec3 const playerPosition = player.m_position;
	EulerAngles const orientation = player.GetCamera().GetOrientation();
	return Stringf(
		"Position: %.1f, %.1f, %.1f Orientation: %.1f %.1f %.1f",
		playerPosition.x,
		playerPosition.y,
		playerPosition.z,
		orientation.m_yawDegrees,
		orientation.m_pitchDegrees,
		orientation.m_rollDegrees);
}

static float GetDebugConfigFloat(char const* key, float defaultValue)
{
	return g_gameConfigBlackboard.GetValue(key, defaultValue);
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
	Texture* cubeTexture = nullptr;
	Texture* sphereTexture = GetTexture(SCENE_SPHERE_TEXTURE_PATH);

	m_player = new Player(this);
	m_player->m_position = Vec3(-2.f, 0.f, 1.f);
	AddEntity(m_player);

	Prop* firstCube = CreateCubeProp(this, SCENE_CUBE_SIDE_LENGTH, cubeTexture);
	firstCube->m_position = Vec3(2.f, 2.f, 0.f);
	firstCube->m_angularVelocity = EulerAngles(
		0.f,
		SCENE_PRIMARY_CUBE_ROTATE_SPEED_DEGREES,
		SCENE_PRIMARY_CUBE_ROTATE_SPEED_DEGREES);
	m_primaryCube = firstCube;
	AddEntity(firstCube);

	Prop* secondCube = CreateCubeProp(this, SCENE_CUBE_SIDE_LENGTH, cubeTexture);
	secondCube->m_position = Vec3(-2.f, -2.f, 0.f);
	m_secondaryCube = secondCube;
	AddEntity(secondCube);

	Prop* sphere = CreateSphereProp(
		this,
		SCENE_SPHERE_RADIUS,
		SCENE_SPHERE_NUM_SLICES,
		SCENE_SPHERE_NUM_STACKS,
		sphereTexture);
	sphere->m_position = Vec3(10.f, -5.f, 1.f);
	sphere->m_angularVelocity = EulerAngles(SCENE_SPHERE_ROTATE_Z_SPEED_DEGREES, 0.f, 0.f);
	AddEntity(sphere);

	Prop* grid = CreateGridProp(this, GridPropConfig());
	AddEntity(grid);

	DebugAddWorldBasis();
}

void Game::Shutdown()
{
	const int entityCount = static_cast<int>(m_entities.size());
	for (int entityIndex = 0; entityIndex < entityCount; ++entityIndex) {
		delete m_entities[entityIndex];
		m_entities[entityIndex] = nullptr;
	}

	m_entities.clear();
	m_player = nullptr;
	m_primaryCube = nullptr;
	m_secondaryCube = nullptr;
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
	UpdateAnimatedSceneProps();

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
	if (g_engine == nullptr || g_engine->m_input == nullptr) {
		return;
	}

	InputSystem* inputSystem = g_engine->m_input;

	// Handle ESC
	if (inputSystem->WasKeyJustPressed(KEYCODE_ESC)) {
		m_nextGameState = GameStates::ATTRACT;
	}

	if (m_player == nullptr) {
		return;
	}

	Vec3 const playerPosition = m_player->m_position;
	Vec3 playerForward = GetPlayerForwardDirection(*m_player);
	if (playerForward.GetLengthSquared() == 0.f) {
		playerForward = Vec3(1.f, 0.f, 0.f);
	}

	if (inputSystem->WasKeyJustPressed('1')) {
		float debugCylinderLength = GetDebugConfigFloat("debugCylinderLength", 20.f);
		float debugCylinderRadius = GetDebugConfigFloat("debugCylinderRadius", 0.0625f);
		float debugCylinderDuration = GetDebugConfigFloat("debugCylinderDuration", 10.f);
		DebugAddWorldCylinder(
			playerPosition,
			playerPosition + (playerForward * debugCylinderLength),
			debugCylinderRadius,
			debugCylinderDuration,
			Rgba8::YELLOW,
			Rgba8::YELLOW,
			DebugRenderMode::XRAY);
	}

	if (inputSystem->IsKeyDown('2')) {
		float debugSphereRadius = GetDebugConfigFloat("debugWorldSphereRadius", 0.25f);
		float debugSphereDuration = GetDebugConfigFloat("debugWorldSphereDuration", 60.f);
		DebugAddWorldSphere(
			Vec3(playerPosition.x, playerPosition.y, 0.f),
			debugSphereRadius,
			debugSphereDuration,
			Rgba8(150, 75, 0, 255),
			Rgba8(150, 75, 0, 255),
			DebugRenderMode::USE_DEPTH);
	}

	if (inputSystem->WasKeyJustPressed('3')) {
		float debugWireSphereForwardOffset =
			GetDebugConfigFloat("debugWireSphereForwardOffset", 2.f);
		float debugWireSphereRadius = GetDebugConfigFloat("debugWireSphereRadius", 1.f);
		float debugWireSphereDuration = GetDebugConfigFloat("debugWireSphereDuration", 5.f);
		DebugAddWorldWireSphere(
			playerPosition + (playerForward * debugWireSphereForwardOffset),
			debugWireSphereRadius,
			debugWireSphereDuration,
			Rgba8::GREEN,
			Rgba8::RED,
			DebugRenderMode::USE_DEPTH);
	}

	if (inputSystem->WasKeyJustPressed('4')) {
		float debugBasisDuration = GetDebugConfigFloat("debugBasisDuration", 20.f);
		float debugBasisAxisLength = GetDebugConfigFloat("debugBasisAxisLength", 1.f);
		float debugBasisAxisThickness = GetDebugConfigFloat("debugBasisAxisThickness", 0.12f);
		float debugBasisTextHeight = GetDebugConfigFloat("debugBasisTextHeight", 1.f);
		float debugBasisTextDuration = GetDebugConfigFloat("debugBasisTextDuration", 1.f);
		DebugAddBasis(
			m_player->GetModelToWorldTransform(),
			debugBasisDuration,
			debugBasisAxisLength,
			debugBasisAxisThickness,
			debugBasisTextHeight,
			debugBasisTextDuration,
			DebugRenderMode::USE_DEPTH);
	}

	if (inputSystem->WasKeyJustPressed('5')) {
		Camera const& playerCamera = m_player->GetCamera();
		Vec3 cameraForward;
		Vec3 cameraLeft;
		Vec3 cameraUp;
		playerCamera.GetOrientation().GetAsVectors_IFwd_JLeft_KUp(cameraForward, cameraLeft, cameraUp);
		(void)cameraLeft;
		cameraForward = cameraForward.GetNormalized();
		cameraUp = cameraUp.GetNormalized();
		if (cameraForward.GetLengthSquared() == 0.f) {
			cameraForward = Vec3(1.f, 0.f, 0.f);
		}
		if (cameraUp.GetLengthSquared() == 0.f) {
			cameraUp = Vec3(0.f, 0.f, 1.f);
		}

		float debugBillboardForwardOffset =
			GetDebugConfigFloat("debugBillboardForwardOffset", 2.0f);
		float debugBillboardUpOffset = GetDebugConfigFloat("debugBillboardUpOffset", 0.15f);
		float debugBillboardTextHeight = GetDebugConfigFloat("debugBillboardTextHeight", 0.125f);
		float debugBillboardDuration = GetDebugConfigFloat("debugBillboardDuration", 10.f);
		Vec3 const textOrigin =
			playerCamera.GetPosition() + (cameraForward * debugBillboardForwardOffset) +
			(cameraUp * debugBillboardUpOffset);
		DebugAddWorldBillboardText(
			BuildPlayerPoseDebugText(*m_player),
			textOrigin,
			debugBillboardTextHeight,
			Vec2(0.5f, 0.5f),
			debugBillboardDuration,
			Rgba8::WHITE,
			Rgba8::RED,
			DebugRenderMode::USE_DEPTH);
	}

	if (inputSystem->WasKeyJustPressed('6')) {
		float debugWireCylinderHeight = GetDebugConfigFloat("debugWireCylinderHeight", 1.f);
		float debugWireCylinderRadius = GetDebugConfigFloat("debugWireCylinderRadius", 0.5f);
		float debugWireCylinderDuration = GetDebugConfigFloat("debugWireCylinderDuration", 10.f);
		DebugAddWorldWireCylinder(
			playerPosition,
			playerPosition + Vec3(0.f, 0.f, debugWireCylinderHeight),
			debugWireCylinderRadius,
			debugWireCylinderDuration,
			Rgba8::WHITE,
			Rgba8::RED,
			DebugRenderMode::USE_DEPTH);
	}

	if (inputSystem->WasKeyJustPressed('7')) {
		float debugMessageDuration = GetDebugConfigFloat("debugMessageDuration", 5.f);
		EulerAngles const cameraOrientation = m_player->GetCamera().GetOrientation();
		DebugAddMessage(
			Stringf(
				"Camera orientation: %.1f %.1f %.1f",
				cameraOrientation.m_yawDegrees,
				cameraOrientation.m_pitchDegrees,
				cameraOrientation.m_rollDegrees),
			debugMessageDuration,
			Rgba8::WHITE,
			Rgba8::WHITE);
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
	if (m_player != nullptr) {
		Vec3 const playerPosition = m_player->m_position;
		std::string playerPositionMessage = Stringf(
			"Player position: %.2f, %.2f, %.2f",
			playerPosition.x,
			playerPosition.y,
			playerPosition.z);
		DebugAddScreenText(
			playerPositionMessage,
			AABB2(10.f, SCREEN_SIZE_Y - 30.f, 780.f, SCREEN_SIZE_Y - 5.f),
			10.f,
			Vec2(0.f, 1.f),
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
	g_engine->m_render->EndCamera(*m_screenCamera);
}

float Game::GetDeltaSeconds() const
{
	return static_cast<float>(m_gameClock.GetDeltaSeconds());
}


void Game::UpdateAnimatedSceneProps()
{
	if (m_secondaryCube == nullptr) {
		return;
	}

	float elapsedSeconds = static_cast<float>(m_gameClock.GetTotalSeconds());
	float brightness = 0.5f * (SinDegrees(elapsedSeconds * 90.f) + 1.f);
	m_secondaryCube->m_color = Interpolate(Rgba8::BLACK, Rgba8::WHITE, brightness);
}



