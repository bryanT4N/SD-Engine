#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/MathUtils.hpp"

//-----------------------------------------------------------------------------------------------

Capsule2::Capsule2(Capsule2 const& copyFrom)
{
	m_boneStart	= copyFrom.m_boneStart;
	m_boneEnd	= copyFrom.m_boneEnd;
	m_radius	= copyFrom.m_radius;
}

Capsule2::Capsule2(Vec2 const& boneStart, Vec2 const& boneEnd, float radius)
{
	m_boneStart	= boneStart;
	m_boneEnd	= boneEnd;
	m_radius	= radius;
}

bool Capsule2::IsPointInside(Vec2 const& point) const
{
	return IsPointInsideCapsule2D(point, m_boneStart, m_boneEnd, m_radius);
}

Vec2 const Capsule2::GetBoneCenter() const
{
	return 0.5f * (m_boneStart + m_boneEnd);
}

float Capsule2::GetBoneLength() const
{
	return GetDistance2D(m_boneStart, m_boneEnd);
}

Vec2 const Capsule2::GetNearestPoint(Vec2 const& referencePosition) const
{
	return GetNearestPointOnCapsule2D(referencePosition, m_boneStart, m_boneEnd, m_radius);
}

Vec2 const Capsule2::GetBoundingDiscCenter() const
{
	return GetBoneCenter();
}

float Capsule2::GetBoundingDiscRadius() const
{
	return 0.5f * GetBoneLength() + m_radius;
}
