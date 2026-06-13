#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Narrative/DialogueDatabase.hpp"

#include <vector>

//-----------------------------------------------------------------------------------------------
class Camera;
class Entity;
class Player;
class Prop;
class DialogueScreen;

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
	std::vector<Entity*>	m_entities;
	DialogueScreen*			m_dialogueScreen	= nullptr;
	DialogueDatabase		m_dialogueDatabase;
	NamedStrings			m_worldState;


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
	float GetDeltaSeconds() const;

};
