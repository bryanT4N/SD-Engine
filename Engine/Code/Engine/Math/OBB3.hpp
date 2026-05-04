#pragma once
#include "Engine/Math/Vec3.hpp"

struct EulerAngles;

//-----------------------------------------------------------------------------------------------
struct OBB3
{
public:
	Vec3 m_iBasis;
	Vec3 m_jBasis;
	Vec3 m_kBasis;
	Vec3 m_halfDimensions;
	Vec3 m_center;

public:
	OBB3();
	explicit OBB3(Vec3 const& center, Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis, Vec3 const& halfDimensions);

	void GetCornerPositions(Vec3 (&out_corners)[8]) const;
	void SetOrientationFromEulerAngles(EulerAngles const& euler);
};
