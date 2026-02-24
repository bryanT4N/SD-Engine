#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"

#define _USE_MATH_DEFINES
#include <cmath>

//-----------------------------------------------------------------------------------------------

float GetClamped(float value, float minValue, float maxValue)
{
	if (value < minValue) return minValue;
	if (value > maxValue) return maxValue;
	return value;
}

float GetClampedZeroToOne(float value)
{
	if (value < 0.f) return 0.f;
	if (value > 1.f) return 1.f;
	return value;
}

float Interpolate(float start, float end, float fractionTowardEnd)
{
	return start + fractionTowardEnd * (end - start);
}

float NormalizeByte( unsigned char byteValue )
{
	return static_cast<float>( byteValue ) / 255.0f;
}

unsigned char DenormalizeByte( float zeroToOne )
{
	float clamped = GetClampedZeroToOne( zeroToOne );
	unsigned int bucket = static_cast<unsigned int>( clamped * 256.0f );
	if( bucket > 255 ) {
		bucket = 255;
	}
	return static_cast<unsigned char>( bucket );
}

float GetFractionWithinRange(float value, float rangeStart, float rangeEnd)
{
	return (value - rangeStart) / (rangeEnd - rangeStart);
}

float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	return Interpolate(outStart, outEnd,
		GetFractionWithinRange(inValue, inStart, inEnd));
}

float RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	if (inValue < inStart) return outStart;
	if (inValue > inEnd) return outEnd;
	return RangeMap(inValue, inStart, inEnd, outStart, outEnd);
}

int RoundDownToInt(float value)
{
	return static_cast<int>(std::floor(value));
}

float ConvertDegreesToRadians(float degrees)
{
	return degrees * static_cast<float>(M_PI) / 180.f;
}

float ConvertRadiansToDegrees(float radians)
{
	return radians * 180.f / static_cast<float>(M_PI);
}

float CosDegrees(float degrees)
{
	return cosf(ConvertDegreesToRadians(degrees));
}

float SinDegrees(float degrees)
{
	return sinf(ConvertDegreesToRadians(degrees));
}

float Atan2Degrees(float y, float x)
{
	return ConvertRadiansToDegrees(atan2f(y, x));
}

//-----------------------------------------------------------------------------------------------
float GetShortestAngularDispDegrees(float startDegrees, float endDegrees)
{
	float d1 = fmodf(startDegrees, 360.f),
		d2 = fmodf(endDegrees, 360.f);
	
	d1 = d1 > 0.f ? d1 : d1 + 360.f;
	d2 = d2 > 0.f ? d2 : d2 + 360.f;

	float angularDispDegrees = d2 - d1;
	float angularDispDegreesAbs = abs(angularDispDegrees);

	return angularDispDegreesAbs < 180.f ? angularDispDegrees : copysign(360.f - angularDispDegreesAbs, - angularDispDegrees);
}

//-----------------------------------------------------------------------------------------------
float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees)
{
	goalDegrees = currentDegrees + GetShortestAngularDispDegrees(currentDegrees, goalDegrees);
	float turnedDegrees = fmodf(GetClamped(goalDegrees, currentDegrees - maxDeltaDegrees, currentDegrees + maxDeltaDegrees), 360.f);
	return turnedDegrees > 0 ? turnedDegrees : turnedDegrees + 360.f;
}

//-----------------------------------------------------------------------------------------------
float GetAngleDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b)
{
	float productOfLengths = a.GetLength() * b.GetLength();
	if (productOfLengths == 0) { return 0.f; }

	float cosTheta = DotProduct2D(a, b) / productOfLengths;
	cosTheta = GetClamped(cosTheta, -1.f, 1.f);
	return ConvertRadiansToDegrees(acosf(cosTheta));
}

//-----------------------------------------------------------------------------------------------
float DotProduct2D(Vec2 const& a, Vec2 const& b)
{
	return a.x * b.x + a.y * b.y;
}

//-----------------------------------------------------------------------------------------------
float DotProduct3D( Vec3 const& a, Vec3 const& b )
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

//-----------------------------------------------------------------------------------------------
float DotProduct4D( Vec4 const& a, Vec4 const& b )
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

//-----------------------------------------------------------------------------------------------
float CrossProduct2D( Vec2 const& a, Vec2 const& b )
{
	return ( a.x * b.y ) - ( a.y * b.x );
}

//-----------------------------------------------------------------------------------------------
Vec3 CrossProduct3D( Vec3 const& a, Vec3 const& b )
{
	return Vec3(
		( a.y * b.z ) - ( a.z * b.y ),
		( a.z * b.x ) - ( a.x * b.z ),
		( a.x * b.y ) - ( a.y * b.x )
	);
}

//-----------------------------------------------------------------------------------------------
float GetDistance2D(Vec2 const& positionA, Vec2 const& positionB)
{
	return (positionB - positionA).GetLength();
}

//-----------------------------------------------------------------------------------------------
float GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB)
{
	return (positionB - positionA).GetLengthSquared();
}

//-----------------------------------------------------------------------------------------------
float GetDistance3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return (positionB - positionA).GetLength();
}

//-----------------------------------------------------------------------------------------------
float GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return (positionB - positionA).GetLengthSquared();
}

//-----------------------------------------------------------------------------------------------
float GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return (positionB - positionA).GetLengthXY();
}

//-----------------------------------------------------------------------------------------------
float GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return (positionB - positionA).GetLengthXYSquared();
}

//-----------------------------------------------------------------------------------------------
int GetTaxicabDistance2D(IntVec2 const& pointA, IntVec2 const& pointB)
{
	int diffX = pointA.x - pointB.x,
		diffY = pointA.y - pointB.y;
	return abs(diffX) + abs(diffY);
}

//-----------------------------------------------------------------------------------------------
float GetProjectedLength2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto)
{
	return  DotProduct2D(vectorToProject, vectorToProjectOnto.GetNormalized());
}

//-----------------------------------------------------------------------------------------------
Vec2 GetProjectedVector2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto)
{
	return  vectorToProjectOnto.GetNormalized() * DotProduct2D(vectorToProject, vectorToProjectOnto.GetNormalized());
}

//-----------------------------------------------------------------------------------------------
bool DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB)
{
	return (radiusA + radiusB) * (radiusA + radiusB) > (centerA - centerB).GetLengthSquared();
}

//-----------------------------------------------------------------------------------------------
bool DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB)
{
	return (radiusA + radiusB) * (radiusA + radiusB) > (centerA - centerB).GetLengthSquared();
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideAABB2D(Vec2 point, AABB2 const& alignedBox)
{
	return (point.x >= alignedBox.m_mins.x && point.x <= alignedBox.m_maxs.x) &&
		(point.y >= alignedBox.m_mins.y && point.y <= alignedBox.m_maxs.y);
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideOBB2D(Vec2 point, OBB2 const& orientedBox)
{
	Vec2 iBasis = orientedBox.m_iBasisNormal;
	Vec2 jBasis = iBasis.GetRotatedBy90Degrees();

	Vec2 cp = point - orientedBox.m_center;
	float cpi = DotProduct2D(cp, iBasis);
	float cpj = DotProduct2D(cp, jBasis);

	if (cpi >= orientedBox.m_halfDimensions.x) return false;
	if (cpi <= -orientedBox.m_halfDimensions.x) return false;
	if (cpj >= orientedBox.m_halfDimensions.y) return false;
	if (cpj <= -orientedBox.m_halfDimensions.y) return false;

	return true;
}

bool IsPointInsideCapsule2D(Vec2 point, Vec2 boneStart, Vec2 boneEnd, float radius)
{
	// OPTIMIZE: Devide into 3 regions, and use normal to exclude
	Vec2 nearestPoint = GetNearestPointOnLineSegment2D(point, boneStart, boneEnd);
	return (point - nearestPoint).GetLengthSquared() <= (radius * radius);
}

bool IsPointInsideTriangle2D(Vec2 point, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2)
{
	Vec2 sideAB = ccw1 - ccw0;
	Vec2 outPerpendicularAB = sideAB.GetRotatedByMinus90Degrees();
	Vec2 sideBC = ccw2 - ccw1;
	Vec2 outPerpendicularBC = sideBC.GetRotatedByMinus90Degrees();
	Vec2 sideCA = ccw0 - ccw2;
	Vec2 outPerpendicularCA = sideCA.GetRotatedByMinus90Degrees();

	Vec2 aP = point - ccw0;
	Vec2 bP = point - ccw1;
	Vec2 cP = point - ccw2;

	// A/B/C
	if (DotProduct2D(sideAB, aP) <= 0.f && DotProduct2D(-sideCA, aP) <= 0.f) return false;
	if (DotProduct2D(sideBC, bP) <= 0.f && DotProduct2D(-sideAB, bP) <= 0.f) return false;
	if (DotProduct2D(sideCA, cP) <= 0.f && DotProduct2D(-sideBC, cP) <= 0.f) return false;

	// AB/BC/CA
	if (DotProduct2D(outPerpendicularAB, aP) >= 0 && DotProduct2D(outPerpendicularAB, bP) >= 0)
		return false;
	if (DotProduct2D(outPerpendicularBC, bP) >= 0 && DotProduct2D(outPerpendicularBC, cP) >= 0)
		return false;
	if (DotProduct2D(outPerpendicularCA, cP) >= 0 && DotProduct2D(outPerpendicularCA, aP) >= 0)
		return false;

	return true;
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float const& discRadius)
{
	return (point - discCenter).GetLength() < discRadius;
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float sectorFwdDegrees, float sectorApertureDegrees, float sectorRadius)
{
	Vec2	v			= point - sectorTip;
	float	degrees		= Atan2Degrees(v.y, v.x);

	if (v.GetLengthSquared() < sectorRadius * sectorRadius) {
		if (std::abs(GetShortestAngularDispDegrees(degrees, sectorFwdDegrees)) < 0.5f * sectorApertureDegrees) {
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorFwdNormal, float sectorApertureDegrees, float sectorRadius)
{
	Vec2	v					= point - sectorTip;
	float	degrees				= Atan2Degrees(v.y, v.x);
	float	sectorFwdDegrees	= sectorFwdNormal.GetOrientationDegrees();

	if (v.GetLengthSquared() < sectorRadius * sectorRadius) {
		if (std::abs(GetShortestAngularDispDegrees(degrees, sectorFwdDegrees)) < 0.5f * sectorApertureDegrees) {
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnDisc2D(Vec2 const& referencePos, Vec2 const& discCenter, float discRadius)
{
	if (IsPointInsideDisc2D(referencePos, discCenter, discRadius)) {
		return referencePos;
	}
	float pointDirectionDegrees = (referencePos - discCenter).GetOrientationDegrees();
	return (discCenter + Vec2::MakeFromPolarDegrees(pointDirectionDegrees, discRadius));
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnAABB2D(Vec2 const& referencePos, AABB2 const& aabb)
{
	return aabb.GetNearestPoint(referencePos);
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnOBB2D(Vec2 const& referencePos, OBB2 const& orientedBox)
{
	if (IsPointInsideOBB2D(referencePos, orientedBox)) {
		return referencePos;
	}

	Vec2 iBasis = orientedBox.m_iBasisNormal;
	Vec2 jBasis = iBasis.GetRotatedBy90Degrees();

	Vec2 cp = referencePos - orientedBox.m_center;
	float cpi = DotProduct2D(cp, iBasis);
	float cpj = DotProduct2D(cp, jBasis);

	cpi = GetClamped(cpi, -orientedBox.m_halfDimensions.x, orientedBox.m_halfDimensions.x);
	cpj = GetClamped(cpj, -orientedBox.m_halfDimensions.y, orientedBox.m_halfDimensions.y);

	return orientedBox.m_center + cpi * iBasis + cpj * jBasis;
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnInfiniteLine2D(Vec2 const& referencePos, Vec2 start, Vec2 end)
{
	float proportion = DotProduct2D(referencePos - start, end - start) / DotProduct2D(end - start, end - start);
	return start + proportion * (end - start);
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnLineSegment2D(Vec2 const& referencePos, Vec2 start, Vec2 end)
{
	float proportion = DotProduct2D(referencePos - start, end - start) / DotProduct2D(end - start, end - start);
	proportion = GetClamped(proportion, 0.f, 1.f);
	return start + proportion * (end - start);
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnCapsule2D(Vec2 const& referencePos, Vec2 boneStart, Vec2 boneEnd, float radius)
{
	if (IsPointInsideCapsule2D(referencePos, boneStart, boneEnd, radius)) {
		return referencePos;
	}

	Vec2 nearestOnBone	= GetNearestPointOnLineSegment2D(referencePos, boneStart, boneEnd);
	Vec2 dirNormal			= (referencePos - nearestOnBone).GetNormalized();

	return nearestOnBone + radius * dirNormal;
}

Vec2 GetNearestPointOnTriangle2D(Vec2 referencePos, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2)
{
	Vec2 sideAB = ccw1 - ccw0;
	Vec2 outPerpendicularAB = sideAB.GetRotatedByMinus90Degrees();
	Vec2 sideBC = ccw2 - ccw1;
	Vec2 outPerpendicularBC = sideBC.GetRotatedByMinus90Degrees();
	Vec2 sideCA = ccw0 - ccw2;
	Vec2 outPerpendicularCA = sideCA.GetRotatedByMinus90Degrees();

	Vec2 aP = referencePos - ccw0;
	Vec2 bP = referencePos - ccw1;
	Vec2 cP = referencePos - ccw2;

	// A/B/C
	if (DotProduct2D(sideAB, aP) <= 0.f && DotProduct2D(-sideCA, aP) <= 0.f) return ccw0;
	if (DotProduct2D(sideBC, bP) <= 0.f && DotProduct2D(-sideAB, bP) <= 0.f) return ccw1;
	if (DotProduct2D(sideCA, cP) <= 0.f && DotProduct2D(-sideBC, cP) <= 0.f) return ccw2;

	// AB/BC/CA
	if (DotProduct2D(outPerpendicularAB, aP) > 0 && DotProduct2D(outPerpendicularAB, bP) > 0 &&
		DotProduct2D(sideAB, aP) > 0 && DotProduct2D(-sideAB, bP) > 0) 
		return GetNearestPointOnLineSegment2D(referencePos, ccw0, ccw1);
	if (DotProduct2D(outPerpendicularBC, bP) > 0 && DotProduct2D(outPerpendicularBC, cP) > 0 &&
		DotProduct2D(sideBC, bP) > 0 && DotProduct2D(-sideBC, cP) > 0)
		return GetNearestPointOnLineSegment2D(referencePos, ccw1, ccw2);
	if (DotProduct2D(outPerpendicularCA, cP) > 0 && DotProduct2D(outPerpendicularCA, aP) > 0 &&
		DotProduct2D(sideCA, cP) > 0 && DotProduct2D(-sideCA, aP) > 0)
		return GetNearestPointOnLineSegment2D(referencePos, ccw2, ccw0);

	return referencePos;
}

//-----------------------------------------------------------------------------------------------
bool PushDiscOutOfFixedPoint2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint)
{
	Vec2	v = mobileDiscCenter - fixedPoint;
	float	d = v.GetLength();
	if (d > discRadius) {
		return false;
	}
	else {
		mobileDiscCenter = fixedPoint + discRadius * v.GetNormalized();
		return true;
	}
}

//-----------------------------------------------------------------------------------------------
bool PushDiscOutOfFixedDisc2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius)
{
	Vec2	v = mobileDiscCenter - fixedDiscCenter;
	float	d = v.GetLength();
	if (d > discRadius + fixedDiscRadius) {
		return false;
	}
	else {
		mobileDiscCenter = fixedDiscCenter + (discRadius + fixedDiscRadius) * v.GetNormalized();
		return true;
	}
}

//-----------------------------------------------------------------------------------------------
bool PushDiscsOutOfEachOther2D(Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius)
{
	Vec2	v = aCenter - bCenter;
	float	d = v.GetLength();
	if (d > aRadius + bRadius) {
		return false;
	}
	else {
		float distanceA2Push =  Interpolate(0, ((aRadius + bRadius) - d), 0.5f),
			distanceB2Push =  Interpolate(0, ((aRadius + bRadius) - d), 0.5f);
		aCenter = aCenter + distanceA2Push * v.GetNormalized();
		bCenter = bCenter - distanceB2Push * v.GetNormalized();
		return true;
	}
}

//-----------------------------------------------------------------------------------------------
bool PushDiscOutOfFixedAABB2D(Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox)
{
	if (fixedBox.IsPointInside(mobileDiscCenter)) {
		return false;
	}
	else {
		Vec2 nearestPoint = fixedBox.GetNearestPoint(mobileDiscCenter);
		return PushDiscOutOfFixedPoint2D(mobileDiscCenter, discRadius, nearestPoint);
	}
}

//-----------------------------------------------------------------------------------------------
void TransformPosition2D(Vec2& posToTransform, float uniformscale, float rotationDegrees, Vec2 const& translation)
{
	// Scale
	posToTransform = posToTransform * uniformscale;

	// Rotate
	posToTransform = posToTransform.GetRotatedByDegrees(rotationDegrees);

	// Translate
	posToTransform = posToTransform + translation;
}

//-----------------------------------------------------------------------------------------------
void TransformPosition2D(Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation)
{
	posToTransform = (posToTransform.x * iBasis) + (posToTransform.y * jBasis) + translation;
}

//-----------------------------------------------------------------------------------------------
void TransformPositionXY3D(Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation)
{
	// XY-Scale
	posToTransform = posToTransform * Vec3(xyScale, xyScale, 1);

	// Z-Rotate
	posToTransform = posToTransform.GetRotatedAboutZDegrees(zRotationDegrees);

	// XY-Translate
	posToTransform = posToTransform + Vec3(xyTranslation.x, xyTranslation.y, 0);
}

//-----------------------------------------------------------------------------------------------
void TransformPositionXY3D(Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation)
{
	Vec2 posToTransformVec2 = (posToTransform.x * iBasis) + (posToTransform.y * jBasis) + translation;
	posToTransform = Vec3(posToTransformVec2.x, posToTransformVec2.y, posToTransform.z);
}

//-----------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsDisc2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius) {
	Vec2	i		= fwdNormal;
	Vec2	j		= fwdNormal.GetRotatedBy90Degrees();
	Vec2	sc		= discCenter - startPos;
	float	scj		= DotProduct2D(sc, j);

	RaycastResult2D missResult;
	missResult.m_didImpact = false;
	missResult.m_impactDist = maxDist;
	missResult.m_impactPos = startPos + (fwdNormal * maxDist);

	if (scj > discRadius || scj < -discRadius) {
		return missResult;
	}

	float	sci		= DotProduct2D(sc, i);

	RaycastResult2D hitResult;

	hitResult.m_rayStartPos = startPos;
	hitResult.m_rayFwdNormal = fwdNormal;
	hitResult.m_rayMaxLength = maxDist;

	if (sci * sci + scj * scj < discRadius * discRadius) {
		hitResult.m_didImpact		= true;
		hitResult.m_impactDist		= 0.f;
		hitResult.m_impactPos		= startPos;
		hitResult.m_impactNormal	= - fwdNormal;

		return hitResult;
	}
	
	float	a_square = discRadius * discRadius - scj * scj;
	float	a = sqrtf(a_square);

	hitResult.m_impactDist = sci - a;
	if (hitResult.m_impactDist < 0 || hitResult.m_impactDist > maxDist) return missResult;

	hitResult.m_didImpact = true;
	hitResult.m_impactPos = startPos + (fwdNormal * hitResult.m_impactDist);
	hitResult.m_impactNormal = (hitResult.m_impactPos - discCenter).GetNormalized();

	return hitResult;
}

