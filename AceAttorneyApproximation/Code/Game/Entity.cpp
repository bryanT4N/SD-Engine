#include "Engine/Core/Rgba8.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"

Entity::Entity(Game* owner, Vec2 const& startingPosition, Vec2 const& startingVelocity)
	:m_game(owner),
	m_position(startingPosition),
	m_velocity(startingVelocity)
{

}

void Entity::RenderDebug() const
{
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);
	DebugDrawLine(m_position, worldCenter, 0.2f, Rgba8(50, 50, 50, 255), Rgba8(50, 50, 50, 255));
}

void Entity::TakeDamage(int damage)
{
	m_health -= damage;
	if (m_health <= 0)
	{
		Die();
	}
}

void Entity::Die()
{
	m_isDead = true;
	m_isGarbage = true;
}

bool Entity::IsOffscreen() const
{
	if (m_position.x + m_cosmeticRadius < 0 ||
		m_position.x - m_cosmeticRadius > WORLD_SIZE_X ||
		m_position.y + m_cosmeticRadius < 0 ||
		m_position.y - m_cosmeticRadius > WORLD_SIZE_Y ) {
		return true;
	}
	return false;
}

Vec2 Entity::GetForwardNormal() const
{
	return Vec2::MakeFromPolarDegrees(m_orientationDegrees);
}

Vec2 Entity::GetLeftNormal() const
{
	return GetForwardNormal().GetRotatedBy90Degrees();
}

bool Entity::IsAlive() const
{
	return !m_isDead;
}

