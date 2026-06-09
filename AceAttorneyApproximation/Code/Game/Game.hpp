#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

//-----------------------------------------------------------------------------------------------
class Camera;
class Entity;

//-----------------------------------------------------------------------------------------------
class Game
{
public:
	RandomNumberGenerator	m_rng;
	GameStates				m_currentGameState	= GameStates::INVALID;
	GameStates				m_nextGameState		= GameStates::ATTRACT;
	Clock					m_gameClock;

	Camera*					m_worldCamera		= nullptr;
	Camera*					m_screenCamera		= nullptr;
	Camera*					m_attractCamera		= nullptr;


public:
	Game();
	~Game();

	void Render() const;
	void Render_Attract() const;
	void Render_Playing() const;

	void Update();
	void UpdateFromKeyboard();
	void UpdateFromController();

	void UpdateEntities();
	void DeleteGarbageEntities();
	float GetDeltaSeconds() const;

};
