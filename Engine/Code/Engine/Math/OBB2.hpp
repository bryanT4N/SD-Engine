#pragma once
#include "Engine/Math/Vec2.hpp"

//-----------------------------------------------------------------------------------------------
class OBB2 
{
public:
	Vec2 m_center;
	Vec2 m_iBasisNormal;
	Vec2 m_halfDimensions;

public:
	// Construction/Destruction
	OBB2() {}
	OBB2(OBB2 const& copyFrom);
	~OBB2() {}
	explicit OBB2(Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions);

	// Accessors (const methods)
	bool		IsPointInside(Vec2 const& point) const;
	Vec2 const	GetCenter() const;
	Vec2 const	GetDimensions() const;
	Vec2 const	GetNearestPoint(Vec2 const& referencePosition) const;

};