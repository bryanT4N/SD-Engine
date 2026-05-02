#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include <cmath>

//-----------------------------------------------------------------------------------------------

OBB2::OBB2(OBB2 const& copyFrom)
{
	m_center			= copyFrom.m_center;
	m_iBasisNormal		= copyFrom.m_iBasisNormal;
	m_halfDimensions	= copyFrom.m_halfDimensions;
}

OBB2::OBB2(Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions)
{
	m_center			= center;
	m_iBasisNormal		= iBasisNormal;
	m_halfDimensions	= halfDimensions;
}

bool OBB2::IsPointInside(Vec2 const& point) const
{
	return IsPointInsideOBB2D(point, *this);
}

Vec2 const OBB2::GetCenter() const
{
	return m_center;
}

Vec2 const OBB2::GetDimensions() const
{
	return 2.f * m_halfDimensions;
}

Vec2 const OBB2::GetNearestPoint(Vec2 const& referencePosition) const
{
	return GetNearestPointOnOBB2D(referencePosition, *this);
}

Vec2 const OBB2::GetBoundingDiscCenter() const
{
	return m_center;
}

float OBB2::GetBoundingDiscRadius() const
{
	return std::sqrt(m_halfDimensions.x * m_halfDimensions.x + m_halfDimensions.y * m_halfDimensions.y);
}


