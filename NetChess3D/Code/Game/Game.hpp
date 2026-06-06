#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

#include <vector>

//-----------------------------------------------------------------------------------------------
class Camera;
class Entity;
class Player;
class Prop;

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
	Player*					m_player			= nullptr;
	Prop*					m_primaryCube		= nullptr;
	Prop*					m_secondaryCube		= nullptr;
	std::vector<Entity*>	m_entities;


public:
	Game();
	~Game();

	void Startup();
	void Shutdown();
	void AddEntity(Entity* entity);

	void Render() const;
	void Render_Attract() const;
	void Render_Playing() const;

	void Update();
	void UpdateFromKeyboard();
	void UpdateFromController();

	void UpdateEntities();
	void DeleteGarbageEntities();
	void UpdateAnimatedSceneProps();
	float GetDeltaSeconds() const;

	static bool ChessMove_Cmd(EventArgs& args);
};
