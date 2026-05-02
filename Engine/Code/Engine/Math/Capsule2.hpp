#pragma once
#include "Engine/Math/Vec2.hpp"

//-----------------------------------------------------------------------------------------------
class Capsule2
{
public:
	Vec2	m_boneStart;
	Vec2	m_boneEnd;
	float	m_radius		= 0.f;

public:
	// Construction/Destruction
	Capsule2() {}
	Capsule2(Capsule2 const& copyFrom);
	~Capsule2() {}
	explicit Capsule2(Vec2 const& boneStart, Vec2 const& boneEnd, float radius);

	// Accessors (const methods)
	bool		IsPointInside(Vec2 const& point) const;
	Vec2 const	GetBoneCenter() const;
	float		GetBoneLength() const;
	Vec2 const	GetNearestPoint(Vec2 const& referencePosition) const;

	Vec2 const	GetBoundingDiscCenter() const;
	float		GetBoundingDiscRadius() const;
};
