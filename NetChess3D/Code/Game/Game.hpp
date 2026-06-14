#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

#include <vector>

//-----------------------------------------------------------------------------------------------
class Camera;
class ChessMatch;
class Entity;
class Player;
class Prop;
class Shader;
class VertexBuffer;

//-----------------------------------------------------------------------------------------------
class Game
{
public:
	RandomNumberGenerator	m_rng;
	GameStates				m_currentGameState	= GameStates::INVALID;
	GameStates				m_nextGameState		= GameStates::ATTRACT;
	Clock					m_gameClock;

	Camera*					m_screenCamera		= nullptr;
	Camera*					m_attractCamera		= nullptr;
	Camera*					m_povCamera			= nullptr;
	Camera*					m_overheadCamera	= nullptr;
	Player*					m_player			= nullptr;
	std::vector<Entity*>	m_entities;
	Shader*					m_litShader			= nullptr;
	ChessMatch*				m_chessMatch		= nullptr;
	CameraMode				m_currentCameraMode	= CameraMode::FREE_SPECTATOR;
	int						m_debugInt			= 0;
	VertexBuffer*			m_testCubeVBO		= nullptr;
	int						m_testCubeVertexCount = 0;


public:
	Game();
	~Game();

	void Startup();
	void Shutdown();
	void AddEntity(Entity* entity);

	void Render() const;
	void Render_Attract() const;
	void Render_Playing() const;
	void RenderTestCubes() const;
	void CreateTestCubeMesh();

	void Update();
	void UpdateFromKeyboard();
	void UpdateFromController();

	void UpdateEntities();
	void DeleteGarbageEntities();
	float GetDeltaSeconds() const;

	Camera const& GetActiveWorldCamera() const;
	void UpdatePoVCameraForCurrentPlayer();
	void CycleCameraMode();

	static bool ChessMove_Cmd(EventArgs& args);
	static bool ChessOverride_Cmd(EventArgs& args);
};
