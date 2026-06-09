#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex.hpp"

//-----------------------------------------------------------------------------------------------
class Game;

//-----------------------------------------------------------------------------------------------
class Entity 
{
public:
	Vec2	m_position;
	Vec2	m_velocity;
	float	m_orientationDegrees	= 0;
	float	m_angularVelocity		= 0;

	float	m_physicsRadius			= 0;
	float	m_cosmeticRadius		= 0;

	int		m_health				= 1;

	bool	m_isDead				= false;
	bool	m_isGarbage				= false;

	Game*	m_game					= nullptr;

	Vertex* m_localVerts			= nullptr;
	int		m_numLocalVerts			= 0;

public:
	Entity(Game* owner, Vec2 const& startingPosition, Vec2 const& startingVelocity);
	virtual ~Entity() = default;

	virtual void Update() = 0;
	virtual void Render() const = 0;
	virtual void RenderDebug() const;

	virtual void TakeDamage(int damage);
	virtual void Die();

	// Accessors
	virtual bool IsOffscreen() const;
	virtual Vec2 GetForwardNormal() const;
	virtual Vec2 GetLeftNormal() const;
	virtual bool IsAlive() const;
};