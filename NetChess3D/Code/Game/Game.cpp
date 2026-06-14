#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include "Game/ChessBoard.hpp"
#include "Game/ChessMatch.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Game/ChessPlayer.hpp"
#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
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
#include "Engine/Renderer/Shader.hpp"
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
	m_player->m_position = Vec3(-2.f, 0.f, 1.f);
	AddEntity(m_player);

	DebugAddWorldBasis();

	m_litShader = g_engine->m_render->CreateShader("BlinnPhong", VertexType::VERTEX_PCUTBN);

	ChessPieceDefinition::InitializeAllDefinitions();

	m_chessMatch = new ChessMatch();

	float cameraAspect = 2.f;
	if (g_engine != nullptr && g_engine->m_window != nullptr) {
		IntVec2 clientDimensions = g_engine->m_window->GetClientDimensions();
		if (clientDimensions.y > 0) {
			cameraAspect =
				static_cast<float>(clientDimensions.x) /
				static_cast<float>(clientDimensions.y);
		}
	}
	Mat44 cameraToRenderTransform(
		Vec3(0.f, 0.f, 1.f),
		Vec3(-1.f, 0.f, 0.f),
		Vec3(0.f, 1.f, 0.f),
		Vec3(0.f, 0.f, 0.f));

	m_povCamera = new Camera();
	m_povCamera->SetPerspectiveView(cameraAspect, 60.f, 0.1f, 100.f);
	m_povCamera->SetCameraToRenderTransform(cameraToRenderTransform);
	UpdatePoVCameraForCurrentPlayer();

	m_overheadCamera = new Camera();
	m_overheadCamera->SetPerspectiveView(cameraAspect, 60.f, 0.1f, 100.f);
	m_overheadCamera->SetCameraToRenderTransform(cameraToRenderTransform);
	m_overheadCamera->SetPositionAndOrientation(
		Vec3(4.f, 4.f, 13.f),
		EulerAngles(90.f, 89.f, 0.f));
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

	delete m_chessMatch;
	m_chessMatch = nullptr;

	ChessPieceDefinition::DestroyAllDefinitions();

	delete m_povCamera;
	m_povCamera = nullptr;

	delete m_overheadCamera;
	m_overheadCamera = nullptr;
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
	if (m_player != nullptr) {
		m_player->m_isInputEnabled = (m_currentCameraMode == CameraMode::FREE_SPECTATOR);
	}

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
	if (g_engine == nullptr || g_engine->m_input == nullptr) {
		return;
	}

	InputSystem* inputSystem = g_engine->m_input;

	// Handle ESC
	if (inputSystem->WasKeyJustPressed(KEYCODE_ESC)) {
		m_nextGameState = GameStates::ATTRACT;
	}

	if (inputSystem->WasKeyJustPressed(KEYCODE_F4)) {
		CycleCameraMode();
	}

	bool shiftHeld = inputSystem->IsKeyDown(KEYCODE_SHIFT);
	for (int digit = 0; digit <= 9; ++digit) {
		if (inputSystem->WasKeyJustPressed(static_cast<unsigned char>('0' + digit))) {
			m_debugInt = shiftHeld ? (digit + 10) : digit;
		}
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

	Camera const& worldCamera = GetActiveWorldCamera();
	g_engine->m_render->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_engine->m_render->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_engine->m_render->BeginCamera(worldCamera);
	{
		g_engine->m_render->SetPerFrameConstants(
			static_cast<float>(m_gameClock.GetTotalSeconds()), m_debugInt, 0.f);
		if (m_chessMatch != nullptr) {
			g_engine->m_render->BindShader(m_litShader);
			m_chessMatch->Render();
		}
		g_engine->m_render->BindShader(nullptr);
		DebugRenderWorld(worldCamera);
	}
	g_engine->m_render->EndCamera(worldCamera);

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
	char const* cameraModeText = "Auto";
	switch (m_currentCameraMode)
	{
	case CameraMode::POV:				cameraModeText = "Auto";		break;
	case CameraMode::OVERHEAD:			cameraModeText = "Overhead";	break;
	case CameraMode::FREE_SPECTATOR:	cameraModeText = "Free";		break;
	default:														break;
	}
	char const* gameStateText = "First Player's Turn";
	if (m_chessMatch != nullptr) {
		switch (m_chessMatch->m_currentState)
		{
		case ChessGameState::FIRST_PLAYER_TURN:
			gameStateText = "First Player's Turn";
			break;
		case ChessGameState::SECOND_PLAYER_TURN:
			gameStateText = "Second Player's Turn";
			break;
		case ChessGameState::GAME_OVER_PLAYER_0_WINS:
		case ChessGameState::GAME_OVER_PLAYER_1_WINS:
			gameStateText = "Match Completed";
			break;
		}
	}

	DebugAddScreenText(
		"Use the DevConsole (~) to enter commands.",
		AABB2(10.f, SCREEN_SIZE_Y - 30.f, 780.f, SCREEN_SIZE_Y - 5.f),
		10.f,
		Vec2(0.f, 1.f),
		0.f,
		Rgba8::YELLOW,
		Rgba8::YELLOW);
	std::string statusBarText = Stringf(
		"CameraMode (F4): %s | GameState: %s",
		cameraModeText,
		gameStateText);
	DebugAddScreenText(
		statusBarText,
		AABB2(10.f, SCREEN_SIZE_Y - 44.f, 780.f, SCREEN_SIZE_Y - 19.f),
		10.f,
		Vec2(0.f, 1.f),
		0.f,
		Rgba8::CYAN,
		Rgba8::CYAN);

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

Camera const& Game::GetActiveWorldCamera() const
{
	if (m_currentCameraMode == CameraMode::POV && m_povCamera != nullptr) {
		return *m_povCamera;
	}
	if (m_currentCameraMode == CameraMode::OVERHEAD && m_overheadCamera != nullptr) {
		return *m_overheadCamera;
	}
	return m_player->GetCamera();
}

void Game::CycleCameraMode()
{
	Camera const* previousCamera = nullptr;
	if (m_currentCameraMode == CameraMode::POV) {
		previousCamera = m_povCamera;
	}
	else if (m_currentCameraMode == CameraMode::OVERHEAD) {
		previousCamera = m_overheadCamera;
	}

	switch (m_currentCameraMode)
	{
	case CameraMode::POV:				m_currentCameraMode = CameraMode::OVERHEAD;			break;
	case CameraMode::OVERHEAD:			m_currentCameraMode = CameraMode::FREE_SPECTATOR;	break;
	case CameraMode::FREE_SPECTATOR:	m_currentCameraMode = CameraMode::POV;				break;
	default:																				break;
	}

	if (m_currentCameraMode == CameraMode::FREE_SPECTATOR &&
		m_player != nullptr && previousCamera != nullptr) {
		m_player->SnapToPose(
			previousCamera->GetPosition(),
			previousCamera->GetOrientation());
	}
}

void Game::UpdatePoVCameraForCurrentPlayer()
{
	if (m_povCamera == nullptr) {
		return;
	}

	ChessGameState gameState = (m_chessMatch != nullptr)
		? m_chessMatch->m_currentState
		: ChessGameState::FIRST_PLAYER_TURN;
	bool isGameOver =
		gameState == ChessGameState::GAME_OVER_PLAYER_0_WINS ||
		gameState == ChessGameState::GAME_OVER_PLAYER_1_WINS;

	Vec3 cameraPosition;
	EulerAngles cameraOrientation;
	if (isGameOver) {
		cameraPosition = Vec3(4.f, 4.f, 13.f);
		cameraOrientation = EulerAngles(90.f, 89.f, 0.f);
	}
	else if (gameState == ChessGameState::FIRST_PLAYER_TURN) {
		cameraPosition = Vec3(4.f, -3.f, 5.f);
		cameraOrientation = EulerAngles(90.f, 35.f, 0.f);
	}
	else {
		cameraPosition = Vec3(4.f, 11.f, 5.f);
		cameraOrientation = EulerAngles(270.f, 35.f, 0.f);
	}

	m_povCamera->SetPositionAndOrientation(cameraPosition, cameraOrientation);
}


//-----------------------------------------------------------------------------------------------
bool Game::ChessMove_Cmd(EventArgs& args)
{
	std::string fromSquareArg = args.GetValue("from", "");
	std::string toSquareArg = args.GetValue("to", "");

	if (fromSquareArg.empty() || toSquareArg.empty()) {
		if (g_engine != nullptr && g_engine->m_devConsole != nullptr) {
			g_engine->m_devConsole->AddLine(
				DevConsole::LOG_COLOR_ERROR,
				"Illegal move: need from= and to= (e.g. from=e2 to=e4).");
		}
		return true;
	}

	IntVec2 fromSquare = ChessMatch::ParseSquareNotation(fromSquareArg);
	if (fromSquare.x < 0) {
		if (g_engine != nullptr && g_engine->m_devConsole != nullptr) {
			g_engine->m_devConsole->AddLine(
				DevConsole::LOG_COLOR_ERROR,
				Stringf("Illegal move: bad 'from' square: %s", fromSquareArg.c_str()));
		}
		return true;
	}

	IntVec2 toSquare = ChessMatch::ParseSquareNotation(toSquareArg);
	if (toSquare.x < 0) {
		if (g_engine != nullptr && g_engine->m_devConsole != nullptr) {
			g_engine->m_devConsole->AddLine(
				DevConsole::LOG_COLOR_ERROR,
				Stringf("Illegal move: bad 'to' square: %s", toSquareArg.c_str()));
		}
		return true;
	}

	if (g_theApp == nullptr || g_theApp->m_game == nullptr || g_theApp->m_game->m_chessMatch == nullptr) {
		return true;
	}

	ChessMatch* match = g_theApp->m_game->m_chessMatch;
	bool isTeleport = args.GetValue("teleport", false);
	std::string promoteTo = args.GetValue("promoteTo", "");
	std::string errorMessage;
	std::string moveAnnouncement;
	std::string captureAnnouncement;
	std::string victoryAnnouncement;
	bool moveSucceeded = match->TryExecuteMove(
		fromSquare, toSquare, isTeleport, promoteTo, errorMessage,
		&moveAnnouncement, &captureAnnouncement, &victoryAnnouncement);

	if (!moveSucceeded) {
		if (g_engine != nullptr && g_engine->m_devConsole != nullptr) {
			g_engine->m_devConsole->AddLine(DevConsole::LOG_COLOR_ERROR, errorMessage);
		}
		return true;
	}

	if (g_engine == nullptr || g_engine->m_devConsole == nullptr) {
		return true;
	}
	DevConsole* devConsole = g_engine->m_devConsole;

	if (!moveAnnouncement.empty()) {
		devConsole->AddLine(DevConsole::LOG_COLOR_INFO_MAJOR, moveAnnouncement);
	}
	if (!captureAnnouncement.empty()) {
		devConsole->AddLine(DevConsole::LOG_COLOR_WARNING, captureAnnouncement);
	}

	g_theApp->m_game->UpdatePoVCameraForCurrentPlayer();

	if (!victoryAnnouncement.empty()) {
		match->PrintVictoryHeaderToDevConsole(victoryAnnouncement);
		return true;
	}

	match->PrintTurnHeaderToDevConsole();
	return true;
}


//-----------------------------------------------------------------------------------------------
bool Game::ChessOverride_Cmd(EventArgs& args)
{
	std::string boardArg = args.GetValue("board", "");
	if (boardArg.empty()) {
		if (g_engine != nullptr && g_engine->m_devConsole != nullptr) {
			g_engine->m_devConsole->AddLine(
				DevConsole::LOG_COLOR_ERROR,
				"Illegal ChessOverride: need board= (64 chars).");
		}
		return true;
	}

	if (g_theApp == nullptr || g_theApp->m_game == nullptr || g_theApp->m_game->m_chessMatch == nullptr) {
		return true;
	}

	ChessMatch* match = g_theApp->m_game->m_chessMatch;
	std::string errorMessage;
	if (!match->ApplyOverride(boardArg, errorMessage)) {
		if (g_engine != nullptr && g_engine->m_devConsole != nullptr) {
			g_engine->m_devConsole->AddLine(DevConsole::LOG_COLOR_ERROR, errorMessage);
		}
		return true;
	}

	if (g_engine != nullptr && g_engine->m_devConsole != nullptr) {
		g_engine->m_devConsole->AddLine(DevConsole::LOG_COLOR_INFO_MAJOR, "Board overridden.");
	}
	match->PrintBoardToDevConsole();
	return true;
}



