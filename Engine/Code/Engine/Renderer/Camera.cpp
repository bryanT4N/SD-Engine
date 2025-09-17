#include "Engine/Renderer/Camera.hpp"

void Camera::SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight)
{
	m_orthoBottomLeft = bottomLeft;
	m_orthoTopRight = topRight;
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return m_orthoBottomLeft;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return m_orthoTopRight;
}
