#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cmath>

//-----------------------------------------------------------------------------------------------

AABB2::AABB2(AABB2 const& copyFrom)
{
	m_mins = copyFrom.m_mins;
	m_maxs = copyFrom.m_maxs;
}

AABB2::AABB2(float minX, float minY, float maxX, float maxY)
{
	m_mins = Vec2(minX, minY);
	m_maxs = Vec2(maxX, maxY);
}

AABB2::AABB2(Vec2 const& mins, Vec2 const& maxs)
{
	m_mins = mins;
	m_maxs = maxs;
}

bool AABB2::IsPointInside(Vec2 const& point) const
{
	return point.x > m_mins.x && point.y > m_mins.y &&
		point.x < m_maxs.x && point.y < m_maxs.y;
}

Vec2 const AABB2::GetCenter() const
{
	return Vec2(0.5f * (m_mins.x + m_maxs.x), 0.5f * (m_mins.y + m_maxs.y));
}

Vec2 const AABB2::GetDimensions() const
{
	return Vec2(m_maxs.x - m_mins.x, m_maxs.y - m_mins.y);
}

Vec2 const AABB2::GetNearestPoint(Vec2 const& referencePosition) const
{
	return Vec2(GetClamped(referencePosition.x, m_mins.x, m_maxs.x), GetClamped(referencePosition.y, m_mins.y, m_maxs.y));
}

Vec2 const AABB2::GetPointAtUV(Vec2 const& uv) const
{
	return Vec2(RangeMap(uv.x, 0.f, 1.f, m_mins.x, m_maxs.x), RangeMap(uv.y, 0.f, 1.f, m_mins.y, m_maxs.y));
}

Vec2 const AABB2::GetUVForPoint(Vec2 const& point) const
{
	return Vec2(RangeMap(point.x, m_mins.x, m_maxs.x, 0.f, 1.f), RangeMap(point.y, m_mins.y, m_maxs.y, 0.f, 1.f));
}

void AABB2::Translate(Vec2 const& translationToApply)
{
	m_mins.x += translationToApply.x;
	m_mins.y += translationToApply.y;
	m_maxs.x += translationToApply.x;
	m_maxs.y += translationToApply.y;
}

void AABB2::SetCenter(Vec2 const& newCenter)
{
	Translate(newCenter - GetCenter());
}

void AABB2::SetDimensions(Vec2 const& newDimensions)
{
	Vec2 center = GetCenter();
	float scaleX = newDimensions.x / GetDimensions().x,
		scaleY = newDimensions.y / GetDimensions().y;
	m_mins.x = Interpolate(center.x, m_mins.x, scaleX);
	m_mins.y = Interpolate(center.y, m_mins.y, scaleY);
	m_maxs.x = Interpolate(center.x, m_maxs.x, scaleX);
	m_maxs.y = Interpolate(center.y, m_maxs.y, scaleY);
}

void AABB2::StretchToIncludePoint(Vec2 const& point)
{
	if (IsPointInside(point)) return;

	float newMinX = point.x < m_mins.x ? point.x : m_mins.x,
		newMinY = point.y < m_mins.y ? point.y : m_mins.y,
		newMaxX = point.x > m_maxs.x ? point.x : m_maxs.x,
		newMaxY = point.y > m_maxs.y ? point.y : m_maxs.y;

	m_mins.x = newMinX;
	m_mins.y = newMinY;
	m_maxs.x = newMaxX;
	m_maxs.y = newMaxY;

// 	Vec2 aabbCenter = GetCenter();
// 	Vec2 nearestPoint = GetNearestPoint(point);
// 	Vec2 scale = Vec2(GetFractionWithinRange(point.x, aabbCenter.x, nearestPoint.x),
// 		GetFractionWithinRange(point.y, aabbCenter.y, nearestPoint.y));
// 	scale.x = scale.x > 1.f ? scale.x : 1.f;
// 	scale.y = scale.y > 1.f ? scale.y : 1.f;
// 	SetDimensions(scale * GetDimensions());
}

