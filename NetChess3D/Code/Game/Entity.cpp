#include "Game/Entity.hpp"

Entity::Entity(Game* owner)
	: m_game(owner)
{
}

Entity::~Entity()
{
}


Mat44 Entity::GetModelToWorldTransform() const
{
	Mat44 modelToWorld = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	modelToWorld.SetTranslation3D(m_position);
	return modelToWorld;
}

