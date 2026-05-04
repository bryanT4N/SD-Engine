#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/EulerAngles.hpp"

//-----------------------------------------------------------------------------------------------

OBB3::OBB3()
	: m_iBasis(1.f, 0.f, 0.f)
	, m_jBasis(0.f, 1.f, 0.f)
	, m_kBasis(0.f, 0.f, 1.f)
	, m_halfDimensions(0.5f, 0.5f, 0.5f)
	, m_center(0.f, 0.f, 0.f)
{
}

OBB3::OBB3(Vec3 const& center, Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis, Vec3 const& halfDimensions)
	: m_iBasis(iBasis)
	, m_jBasis(jBasis)
	, m_kBasis(kBasis)
	, m_halfDimensions(halfDimensions)
	, m_center(center)
{
}

void OBB3::GetCornerPositions(Vec3 (&out_corners)[8]) const
{
	Vec3 ix = m_iBasis * m_halfDimensions.x;
	Vec3 jy = m_jBasis * m_halfDimensions.y;
	Vec3 kz = m_kBasis * m_halfDimensions.z;

	out_corners[0] = m_center - ix - jy - kz;
	out_corners[1] = m_center + ix - jy - kz;
	out_corners[2] = m_center + ix + jy - kz;
	out_corners[3] = m_center - ix + jy - kz;
	out_corners[4] = m_center - ix - jy + kz;
	out_corners[5] = m_center + ix - jy + kz;
	out_corners[6] = m_center + ix + jy + kz;
	out_corners[7] = m_center - ix + jy + kz;
}

void OBB3::SetOrientationFromEulerAngles(EulerAngles const& euler)
{
	euler.GetAsVectors_IFwd_JLeft_KUp(m_iBasis, m_jBasis, m_kBasis);
}
