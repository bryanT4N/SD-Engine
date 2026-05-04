#include "Engine/Math/Plane3.hpp"

//-----------------------------------------------------------------------------------------------

Plane3::Plane3()
	: m_normal(0.f, 0.f, 1.f)
	, m_distanceFromOrigin(0.f)
{
}

Plane3::Plane3(Vec3 const& normal, float distanceFromOrigin)
	: m_normal(normal)
	, m_distanceFromOrigin(distanceFromOrigin)
{
}
