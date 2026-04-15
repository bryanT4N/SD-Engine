#pragma once
#include "Engine/Math/Vec2.hpp"

//-----------------------------------------------------------------------------------------------
class AABB2 
{
public:
	Vec2 m_mins;
	Vec2 m_maxs;

public:
	// Construction/Destruction
	AABB2() {}
	AABB2(AABB2 const& copyFrom);
	~AABB2() {}
	explicit AABB2(float minX, float minY, float maxX, float maxY);
	explicit AABB2(Vec2 const& mins, Vec2 const& maxs);

	// Accessors (const methods)
	bool		IsPointInside(Vec2 const& point) const;
	Vec2 const	GetCenter() const;
	Vec2 const	GetDimensions() const;
	Vec2 const	GetNearestPoint(Vec2 const& referencePosition) const;
	Vec2 const	GetPointAtUV(Vec2 const& uv) const;		// uv=(0,0) is at mins; uv=(1,1)is at maxs
	Vec2 const	GetUVForPoint(Vec2 const& point )const;	// uv=(.5,.5) at center; u or v outside [0,1] extrapolated
	AABB2 const	GetBoxAtUVs(Vec2 const& uvMins, Vec2 const& uvMaxs) const;
	AABB2 const	GetPadded(float uniformPadding) const;
	AABB2 const	GetPadded(float xPadding, float yPadding) const;
	AABB2 const	GetBottomHalf() const;
	AABB2 const	GetTopHalf() const;
	AABB2 const	GetLeftHalf() const;
	AABB2 const	GetRightHalf() const;

	// Mutators(non-const methods)
	void		Translate(Vec2 const& translationToApply);
	void		SetCenter(Vec2 const& newCenter);
	void		SetDimensions(Vec2 const& newDimensions);
	void		StretchToIncludePoint(Vec2 const& point);		// does minimal stretching required (none if already on point)

	// Operators (const)
	bool		operator==(AABB2 const& compare) const;
	bool		operator!=(AABB2 const& compare) const;
};