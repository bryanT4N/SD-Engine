#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

//-----------------------------------------------------------------------------------------------
// Forward type declarations
struct Vec2;
struct Vec3;
struct Vec4;
struct IntVec2;
struct Mat44;
class AABB2;
class OBB2;
class Capsule2;
struct AABB3;

//-----------------------------------------------------------------------------------------------
// Clamp and lerp
float	GetClamped( float value, float minValue, float maxValue );
float	GetClampedZeroToOne(float value );
float	Interpolate( float start, float end, float fractionTowardEnd );
float	NormalizeByte( unsigned char byteValue );
unsigned char DenormalizeByte( float zeroToOne );
float	GetFractionWithinRange( float value, float rangeStart, float rangeEnd );
float	RangeMap( float inValue, float inStart, float inEnd, float outStart, float outEnd );
float	RangeMapClamped( float inValue, float inStart, float inEnd, float outStart, float outEnd );
int		RoundDownToInt( float value );

//-----------------------------------------------------------------------------------------------
// Bezier and easing utilities (MP2-A05)
float	ComputeCubicBezier1D( float A, float B, float C, float D, float t );
float	ComputeQuinticBezier1D( float A, float B, float C, float D, float E, float F, float t );

float	SmoothStart2( float t );
float	SmoothStart3( float t );
float	SmoothStart4( float t );
float	SmoothStart5( float t );
float	SmoothStart6( float t );

float	SmoothStop2( float t );
float	SmoothStop3( float t );
float	SmoothStop4( float t );
float	SmoothStop5( float t );
float	SmoothStop6( float t );

float	SmoothStep3( float t );
float	SmoothStep5( float t );
float	Hesitate3( float t );
float	Hesitate5( float t );
float	CustomFunkyEasingFunction( float t );

//-----------------------------------------------------------------------------------------------
// Angle utilities
float	ConvertDegreesToRadians(float degrees); 
float	ConvertRadiansToDegrees(float radians); 
float	CosDegrees(float degrees); 
float	SinDegrees(float degrees); 
float	Atan2Degrees(float y, float x);
float	GetShortestAngularDispDegrees( float startDegrees, float endDegrees);
float	GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees);
float	GetAngleDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b);

//-----------------------------------------------------------------------------------------------
// Dot and Cross
float	DotProduct2D( Vec2 const& a,Vec2 const& b );
float	DotProduct3D( Vec3 const& a, Vec3 const& b );
float	DotProduct4D( Vec4 const& a, Vec4 const& b );
float	CrossProduct2D( Vec2 const& a, Vec2 const& b );
Vec3	CrossProduct3D( Vec3 const& a, Vec3 const& b );

//-----------------------------------------------------------------------------------------------
// Distance and projection utilities
float	GetDistance2D(Vec2 const& positionA, Vec2 const& positionB); 
float	GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB); 
float	GetDistance3D(Vec3 const& positionA, Vec3 const& positionB); 
float	GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB); 
float	GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB);
float	GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB);
int		GetTaxicabDistance2D(IntVec2 const& pointA, IntVec2 const& pointB);
float	GetProjectedLength2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto); // Works if Vecs not normalized
Vec2	GetProjectedVector2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto);	// Works if Vecs not normalized

//-----------------------------------------------------------------------------------------------
// Geometric query utilities
bool	DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB); 
bool	DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB);
bool	DoAABB3sOverlap(AABB3 const& aabbA, AABB3 const& aabbB);
bool	DoZCylindersOverlap3D(Vec2 const& centerA, float radiusA, float minZA, float maxZA,
	Vec2 const& centerB, float radiusB, float minZB, float maxZB);
bool	DoSphereAndAABB3Overlap(Vec3 const& sphereCenter, float sphereRadius, AABB3 const& aabb);
bool	DoSphereAndZCylinderOverlap3D(Vec3 const& sphereCenter, float sphereRadius,
	Vec2 const& cylinderCenterXY, float cylinderRadius, float cylinderMinZ, float cylinderMaxZ);
bool	DoAABB3AndZCylinderOverlap3D(AABB3 const& aabb, Vec2 const& cylinderCenterXY,
	float cylinderRadius, float cylinderMinZ, float cylinderMaxZ);

//-----------------------------------------------------------------------------------------------
// Is point inside ?
bool	IsPointInsideAABB2D(Vec2 point, AABB2 const& alignedBox); 
bool	IsPointInsideOBB2D(Vec2 point, OBB2 const& orientedBox);
bool	IsPointInsideCapsule2D(Vec2 point, Vec2 boneStart, Vec2 boneEnd, float radius);
bool	IsPointInsideTriangle2D(Vec2 point, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2);							// counter-clockwise
bool	IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float const& discRadius);
bool	IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip,
	float sectorFwdDegrees, float sectorApertureDegrees, float sectorRadius);
bool	IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip,
	Vec2 const& sectorFwdNormal, float sectorApertureDegrees, float sectorRadius);
bool	IsPointInsideSphere3D(Vec3 const& point, Vec3 const& sphereCenter, float sphereRadius);
bool	IsPointInsideAABB3D(Vec3 const& point, AABB3 const& aabb);
bool	IsPointInsideZCylinder3D(Vec3 const& point, Vec2 const& cylinderCenterXY,
	float cylinderRadius, float cylinderMinZ, float cylinderMaxZ);

//-----------------------------------------------------------------------------------------------
// Get nearest point on ?
Vec2	GetNearestPointOnDisc2D(Vec2 const& referencePos, Vec2 const& discCenter, float discRadius);
Vec2	GetNearestPointOnAABB2D(Vec2 const& referencePos, AABB2 const& aabb);
Vec2	GetNearestPointOnOBB2D(Vec2 const& referencePos, OBB2 const& orientedBox);
Vec2	GetNearestPointOnInfiniteLine2D(Vec2 const& referencePos, Vec2 pointOnLine, Vec2 anotherPointOnLine);
Vec2	GetNearestPointOnLineSegment2D(Vec2 const& referencePos, Vec2 start, Vec2 end);
Vec2	GetNearestPointOnCapsule2D(Vec2 const& referencePos, Vec2 boneStart, Vec2 boneEnd, float radius);
Vec2	GetNearestPointOnTriangle2D(Vec2 referencePos, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2);
Vec3	GetNearestPointOnSphere3D(Vec3 const& referencePos, Vec3 const& sphereCenter, float sphereRadius);
Vec3	GetNearestPointOnAABB3D(Vec3 const& referencePos, AABB3 const& aabb);
Vec3	GetNearestPointOnZCylinder3D(Vec3 const& referencePos, Vec2 const& cylinderCenterXY,
	float cylinderRadius, float cylinderMinZ, float cylinderMaxZ);

//-----------------------------------------------------------------------------------------------
bool	PushDiscOutOfFixedPoint2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint);
bool	PushDiscOutOfFixedDisc2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius);
bool	PushDiscsOutOfEachOther2D(Vec2& aCenter,float aRadius, Vec2& bCenter, float bRadius);
bool	PushDiscOutOfFixedAABB2D(Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox);
bool	PushDiscOutOfFixedCapsule2D(Vec2& mobileDiscCenter, float discRadius, Capsule2 const& fixedCapsule);
bool	PushDiscOutOfFixedOBB2D(Vec2& mobileDiscCenter, float discRadius, OBB2 const& fixedBox);

// Reflection / bounce
Vec2	BounceVectorOffSurface(Vec2 const& incomingVelocity, Vec2 const& surfaceNormal, float elasticity);

//-----------------------------------------------------------------------------------------------
// Transform utilities
void	TransformPosition2D(Vec2& posToTransform, float uniformscale, float rotationDegrees, Vec2 const& translation);
void	TransformPositionXY3D(Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation);
void	TransformPosition2D(Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);
void	TransformPositionXY3D(Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);

enum class BillboardType
{
	NONE = -1,
	WORLD_UP_FACING,
	WORLD_UP_OPPOSING,
	FULL_FACING,
	FULL_OPPOSING,
	COUNT
};

Mat44 GetBillboardTransform(
	BillboardType billboardType,
	Mat44 const& targetTransform,
	Vec3 const& billboardPosition,
	Vec2 const& billboardScale = Vec2(1.0f, 1.0f));

//-----------------------------------------------------------------------------------------------
struct RaycastResult2D {
	// Basic raycast result information (required)
	bool	m_didImpact		= false;
	float	m_impactDist	= 0.f;
	Vec2	m_impactPos		= Vec2::ZERO;
	Vec2	m_impactNormal	= Vec2::ZERO;

	// Original raycast information (optional)
	Vec2	m_rayStartPos	= Vec2::ZERO;
	Vec2	m_rayFwdNormal	= Vec2::ZERO;
	float	m_rayMaxLength	= 1.f;
};

struct RaycastResult3D {
	bool	m_didImpact		= false;
	float	m_impactDist	= 0.f;
	Vec3	m_impactPos;
	Vec3	m_impactNormal;

	Vec3	m_rayStartPos;
	Vec3	m_rayFwdNormal;
	float	m_rayMaxLength	= 1.f;
};

//-----------------------------------------------------------------------------------------------
// Raycast utilities
RaycastResult2D RaycastVsDisc2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius );
RaycastResult2D RaycastVsLineSegment2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 lineStart, Vec2 lineEnd );
RaycastResult2D RaycastVsAABB2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2 const& aabb );
RaycastResult3D RaycastVsSphere3D(Vec3 startPos, Vec3 fwdNormal, float maxDist,
	Vec3 sphereCenter, float sphereRadius);
RaycastResult3D RaycastVsAABB3D(Vec3 startPos, Vec3 fwdNormal, float maxDist,
	AABB3 const& aabb);
RaycastResult3D RaycastVsZCylinder3D(Vec3 startPos, Vec3 fwdNormal, float maxDist,
	Vec2 cylinderCenterXY, float cylinderRadius, float cylinderMinZ, float cylinderMaxZ);