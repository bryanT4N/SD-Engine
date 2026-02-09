#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
static void GetBasis_IFwd_JLeft_KUp( float yawDegrees, float pitchDegrees, float rollDegrees,
	Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis )
{
	float cosYaw = CosDegrees( yawDegrees );
	float sinYaw = SinDegrees( yawDegrees );
	float cosPitch = CosDegrees( pitchDegrees );
	float sinPitch = SinDegrees( pitchDegrees );

	out_forwardIBasis = Vec3( cosYaw * cosPitch, sinYaw * cosPitch, -sinPitch );
	out_leftJBasis = Vec3( -sinYaw, cosYaw, 0.f );
	out_upKBasis = CrossProduct3D( out_forwardIBasis, out_leftJBasis );

	if( rollDegrees != 0.f ) {
		float cosRoll = CosDegrees( rollDegrees );
		float sinRoll = SinDegrees( rollDegrees );
		Vec3 left = out_leftJBasis;
		Vec3 up = out_upKBasis;
		out_leftJBasis = ( left * cosRoll ) + ( up * sinRoll );
		out_upKBasis = ( up * cosRoll ) - ( left * sinRoll );
	}
}


//-----------------------------------------------------------------------------------------------
EulerAngles::EulerAngles( float yawDegrees, float pitchDegrees, float rollDegrees )
	: m_yawDegrees( yawDegrees )
	, m_pitchDegrees( pitchDegrees )
	, m_rollDegrees( rollDegrees )
{
}


//-----------------------------------------------------------------------------------------------
Vec3 EulerAngles::GetForwardDir_IFwd_JLeft_KUp() const
{
	Vec3 forward;
	Vec3 left;
	Vec3 up;
	GetBasis_IFwd_JLeft_KUp( m_yawDegrees, m_pitchDegrees, m_rollDegrees, forward, left, up );
	return forward;
}


//-----------------------------------------------------------------------------------------------
void EulerAngles::GetAsVectors_IFwd_JLeft_KUp( Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis ) const
{
	GetBasis_IFwd_JLeft_KUp( m_yawDegrees, m_pitchDegrees, m_rollDegrees, out_forwardIBasis, out_leftJBasis, out_upKBasis );
}


//-----------------------------------------------------------------------------------------------
Mat44 EulerAngles::GetAsMatrix_IFwd_JLeft_KUp() const
{
	Vec3 forward;
	Vec3 left;
	Vec3 up;
	GetBasis_IFwd_JLeft_KUp( m_yawDegrees, m_pitchDegrees, m_rollDegrees, forward, left, up );
	return Mat44( forward, left, up, Vec3( 0.f, 0.f, 0.f ) );
}


//-----------------------------------------------------------------------------------------------
Mat44 EulerAngles::GetAsMatrix_IRight_JUp_KFwd() const
{
	Vec3 forward;
	Vec3 left;
	Vec3 up;
	GetBasis_IFwd_JLeft_KUp( m_yawDegrees, m_pitchDegrees, m_rollDegrees, forward, left, up );
	return Mat44( -left, up, forward, Vec3( 0.f, 0.f, 0.f ) );
}


//-----------------------------------------------------------------------------------------------
void EulerAngles::operator+=( EulerAngles const& anglesToAdd )
{
	m_yawDegrees += anglesToAdd.m_yawDegrees;
	m_pitchDegrees += anglesToAdd.m_pitchDegrees;
	m_rollDegrees += anglesToAdd.m_rollDegrees;
}


//-----------------------------------------------------------------------------------------------
EulerAngles const Interpolate( EulerAngles const& from, EulerAngles const& to, float lerpFraction )
{
	float yawDelta = GetShortestAngularDispDegrees( from.m_yawDegrees, to.m_yawDegrees );
	float yaw = from.m_yawDegrees + ( yawDelta * lerpFraction );
	float pitch = Interpolate( from.m_pitchDegrees, to.m_pitchDegrees, lerpFraction );
	float roll = Interpolate( from.m_rollDegrees, to.m_rollDegrees, lerpFraction );
	return EulerAngles( yaw, pitch, roll );
}

