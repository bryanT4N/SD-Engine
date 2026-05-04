#pragma once
#include "Engine/Math/Vec3.hpp"

//-----------------------------------------------------------------------------------------------
struct Plane3
{
public:
	Vec3 m_normal;
	float m_distanceFromOrigin;

public:
	Plane3();
	explicit Plane3(Vec3 const& normal, float distanceFromOrigin);
};
