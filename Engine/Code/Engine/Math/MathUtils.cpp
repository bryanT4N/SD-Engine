#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB2.hpp"

#define _USE_MATH_DEFINES
#include <cmath>

//-----------------------------------------------------------------------------------------------
namespace
{
Vec3 GetNormalizedOrFallback(Vec3 const& value, Vec3 const& fallback)
{
	Vec3 normalized = value.GetNormalized();
	if (normalized.GetLengthSquared() == 0.f) {
		return fallback;
	}

	return normalized;
}

bool DoFloatRangesOverlap(float minA, float maxA, float minB, float maxB)
{
	return !(maxA < minB || maxB < minA);
}

Vec2 GetNearestPointOnDisc2DInternal(Vec2 const& referencePos, Vec2 const& discCenter, float discRadius)
{
	Vec2 displacement = referencePos - discCenter;
	float displacementLengthSquared = displacement.GetLengthSquared();
	float radiusSquared = discRadius * discRadius;
	if (displacementLengthSquared <= radiusSquared) {
		return referencePos;
	}

	if (displacementLengthSquared == 0.f) {
		return discCenter + Vec2(discRadius, 0.f);
	}

	return discCenter + displacement.GetNormalized() * discRadius;
}
}

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

float ComputeCubicBezier1D(float A, float B, float C, float D, float t)
{
	float ab = Interpolate(A, B, t);
	float bc = Interpolate(B, C, t);
	float cd = Interpolate(C, D, t);

	float abc = Interpolate(ab, bc, t);
	float bcd = Interpolate(bc, cd, t);

	return Interpolate(abc, bcd, t);
}

float ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t)
{
	float ab = Interpolate(A, B, t);
	float bc = Interpolate(B, C, t);
	float cd = Interpolate(C, D, t);
	float de = Interpolate(D, E, t);
	float ef = Interpolate(E, F, t);

	float abc = Interpolate(ab, bc, t);
	float bcd = Interpolate(bc, cd, t);
	float cde = Interpolate(cd, de, t);
	float def = Interpolate(de, ef, t);

	float abcd = Interpolate(abc, bcd, t);
	float bcde = Interpolate(bcd, cde, t);
	float cdef = Interpolate(cde, def, t);

	float abcde = Interpolate(abcd, bcde, t);
	float bcdef = Interpolate(bcde, cdef, t);

	return Interpolate(abcde, bcdef, t);
}

float SmoothStart2(float t)
{
	return t * t;
}

float SmoothStart3(float t)
{
	float t2 = t * t;
	return t2 * t;
}

float SmoothStart4(float t)
{
	float t2 = t * t;
	return t2 * t2;
}

float SmoothStart5(float t)
{
	float t2 = t * t;
	float t4 = t2 * t2;
	return t4 * t;
}

float SmoothStart6(float t)
{
	float t2 = t * t;
	float t4 = t2 * t2;
	return t4 * t2;
}

float SmoothStop2(float t)
{
	float oneMinusT = 1.f - t;
	float omt2 = oneMinusT * oneMinusT;
	return 1.f - omt2;
}

float SmoothStop3(float t)
{
	float oneMinusT = 1.f - t;
	float omt2 = oneMinusT * oneMinusT;
	float omt3 = omt2 * oneMinusT;
	return 1.f - omt3;
}

float SmoothStop4(float t)
{
	float oneMinusT = 1.f - t;
	float omt2 = oneMinusT * oneMinusT;
	float omt4 = omt2 * omt2;
	return 1.f - omt4;
}

float SmoothStop5(float t)
{
	float oneMinusT = 1.f - t;
	float omt2 = oneMinusT * oneMinusT;
	float omt4 = omt2 * omt2;
	float omt5 = omt4 * oneMinusT;
	return 1.f - omt5;
}

float SmoothStop6(float t)
{
	float oneMinusT = 1.f - t;
	float omt2 = oneMinusT * oneMinusT;
	float omt4 = omt2 * omt2;
	float omt6 = omt4 * omt2;
	return 1.f - omt6;
}

float SmoothStep3(float t)
{
	float t2 = t * t;
	return t2 * (3.f - 2.f * t);
}

float SmoothStep5(float t)
{
	float t2 = t * t;
	float t3 = t2 * t;
	return t3 * (10.f + t * (-15.f + 6.f * t));
}

float Hesitate3(float t)
{
	float t2 = t * t;
	float t3 = t2 * t;
	return (3.f * t) - (6.f * t2) + (4.f * t3);
}

float Hesitate5(float t)
{
	return ComputeQuinticBezier1D(0.f, 1.f, 0.f, 1.f, 0.f, 1.f, t);
}

float CustomFunkyEasingFunction(float t)
{
	float wobble = SinDegrees(360.f * t);
	float blend = t * (1.f - t);
	return GetClampedZeroToOne(t + 0.2f * wobble * blend);
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

bool DoAABB3sOverlap(AABB3 const& aabbA, AABB3 const& aabbB)
{
	return !(aabbA.m_maxs.x < aabbB.m_mins.x || aabbB.m_maxs.x < aabbA.m_mins.x ||
		aabbA.m_maxs.y < aabbB.m_mins.y || aabbB.m_maxs.y < aabbA.m_mins.y ||
		aabbA.m_maxs.z < aabbB.m_mins.z || aabbB.m_maxs.z < aabbA.m_mins.z);
}

bool DoZCylindersOverlap3D(Vec2 const& centerA, float radiusA, float minZA, float maxZA,
	Vec2 const& centerB, float radiusB, float minZB, float maxZB)
{
	if (!DoFloatRangesOverlap(minZA, maxZA, minZB, maxZB)) {
		return false;
	}

	float combinedRadius = radiusA + radiusB;
	return (centerA - centerB).GetLengthSquared() <= (combinedRadius * combinedRadius);
}

bool DoSphereAndAABB3Overlap(Vec3 const& sphereCenter, float sphereRadius, AABB3 const& aabb)
{
	Vec3 nearestPoint = GetNearestPointOnAABB3D(sphereCenter, aabb);
	return GetDistanceSquared3D(sphereCenter, nearestPoint) <= sphereRadius * sphereRadius;
}

bool DoSphereAndZCylinderOverlap3D(Vec3 const& sphereCenter, float sphereRadius,
	Vec2 const& cylinderCenterXY, float cylinderRadius, float cylinderMinZ, float cylinderMaxZ)
{
	Vec3 nearestPoint = GetNearestPointOnZCylinder3D(
		sphereCenter,
		cylinderCenterXY,
		cylinderRadius,
		cylinderMinZ,
		cylinderMaxZ);
	return GetDistanceSquared3D(sphereCenter, nearestPoint) <= sphereRadius * sphereRadius;
}

bool DoAABB3AndZCylinderOverlap3D(AABB3 const& aabb, Vec2 const& cylinderCenterXY,
	float cylinderRadius, float cylinderMinZ, float cylinderMaxZ)
{
	if (!DoFloatRangesOverlap(aabb.m_mins.z, aabb.m_maxs.z, cylinderMinZ, cylinderMaxZ)) {
		return false;
	}

	Vec2 nearestPointOnAABBXY(
		GetClamped(cylinderCenterXY.x, aabb.m_mins.x, aabb.m_maxs.x),
		GetClamped(cylinderCenterXY.y, aabb.m_mins.y, aabb.m_maxs.y));
	return (nearestPointOnAABBXY - cylinderCenterXY).GetLengthSquared() <= (cylinderRadius * cylinderRadius);
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

bool IsPointInsideSphere3D(Vec3 const& point, Vec3 const& sphereCenter, float sphereRadius)
{
	return GetDistanceSquared3D(point, sphereCenter) <= sphereRadius * sphereRadius;
}

bool IsPointInsideAABB3D(Vec3 const& point, AABB3 const& aabb)
{
	return point.x >= aabb.m_mins.x && point.x <= aabb.m_maxs.x &&
		point.y >= aabb.m_mins.y && point.y <= aabb.m_maxs.y &&
		point.z >= aabb.m_mins.z && point.z <= aabb.m_maxs.z;
}

bool IsPointInsideZCylinder3D(Vec3 const& point, Vec2 const& cylinderCenterXY,
	float cylinderRadius, float cylinderMinZ, float cylinderMaxZ)
{
	if (point.z < cylinderMinZ || point.z > cylinderMaxZ) {
		return false;
	}

	Vec2 pointXY(point.x, point.y);
	return GetDistanceSquared2D(pointXY, cylinderCenterXY) <= cylinderRadius * cylinderRadius;
}

//-----------------------------------------------------------------------------------------------
Vec2 GetNearestPointOnDisc2D(Vec2 const& referencePos, Vec2 const& discCenter, float discRadius)
{
	return GetNearestPointOnDisc2DInternal(referencePos, discCenter, discRadius);
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

Vec3 GetNearestPointOnSphere3D(Vec3 const& referencePos, Vec3 const& sphereCenter, float sphereRadius)
{
	Vec3 displacement = referencePos - sphereCenter;
	float displacementLengthSquared = displacement.GetLengthSquared();
	float radiusSquared = sphereRadius * sphereRadius;
	if (displacementLengthSquared <= radiusSquared) {
		return referencePos;
	}

	if (displacementLengthSquared == 0.f) {
		return sphereCenter + Vec3(sphereRadius, 0.f, 0.f);
	}

	return sphereCenter + displacement.GetNormalized() * sphereRadius;
}

Vec3 GetNearestPointOnAABB3D(Vec3 const& referencePos, AABB3 const& aabb)
{
	return Vec3(
		GetClamped(referencePos.x, aabb.m_mins.x, aabb.m_maxs.x),
		GetClamped(referencePos.y, aabb.m_mins.y, aabb.m_maxs.y),
		GetClamped(referencePos.z, aabb.m_mins.z, aabb.m_maxs.z));
}

Vec3 GetNearestPointOnZCylinder3D(Vec3 const& referencePos, Vec2 const& cylinderCenterXY,
	float cylinderRadius, float cylinderMinZ, float cylinderMaxZ)
{
	if (IsPointInsideZCylinder3D(referencePos, cylinderCenterXY, cylinderRadius, cylinderMinZ, cylinderMaxZ)) {
		return referencePos;
	}

	Vec2 pointXY(referencePos.x, referencePos.y);
	Vec2 nearestXY = GetNearestPointOnDisc2DInternal(pointXY, cylinderCenterXY, cylinderRadius);
	float nearestZ = GetClamped(referencePos.z, cylinderMinZ, cylinderMaxZ);
	return Vec3(nearestXY.x, nearestXY.y, nearestZ);
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
Mat44 GetBillboardTransform(
	BillboardType billboardType,
	Mat44 const& targetTransform,
	Vec3 const& billboardPosition,
	Vec2 const& billboardScale)
{
	Vec3 targetForward = GetNormalizedOrFallback(targetTransform.GetIBasis3D(), Vec3(1.f, 0.f, 0.f));
	Vec3 targetLeft = GetNormalizedOrFallback(targetTransform.GetJBasis3D(), Vec3(0.f, 1.f, 0.f));
	Vec3 targetUp = GetNormalizedOrFallback(targetTransform.GetKBasis3D(), Vec3(0.f, 0.f, 1.f));

	Vec3 billboardForward = targetForward;
	Vec3 billboardLeft = targetLeft;
	Vec3 billboardUp = targetUp;

	switch (billboardType)
	{
	case BillboardType::WORLD_UP_FACING:
	case BillboardType::WORLD_UP_OPPOSING:
	{
		Vec3 flattenedForward = Vec3(targetForward.x, targetForward.y, 0.f);
		if (flattenedForward.GetLengthSquared() == 0.f) {
			flattenedForward = Vec3(targetLeft.y, -targetLeft.x, 0.f);
		}
		flattenedForward = GetNormalizedOrFallback(flattenedForward, Vec3(1.f, 0.f, 0.f));

		if (billboardType == BillboardType::WORLD_UP_OPPOSING) {
			flattenedForward = -flattenedForward;
		}

		billboardForward = flattenedForward;
		billboardUp = Vec3(0.f, 0.f, 1.f);
		billboardLeft = GetNormalizedOrFallback(
			CrossProduct3D(billboardUp, billboardForward),
			Vec3(0.f, 1.f, 0.f));
		break;
	}

	case BillboardType::FULL_FACING:
		billboardForward = targetForward;
		billboardLeft = targetLeft;
		billboardUp = targetUp;
		break;

	case BillboardType::FULL_OPPOSING:
		billboardForward = -targetForward;
		billboardLeft = -targetLeft;
		billboardUp = targetUp;
		break;

	case BillboardType::COUNT:
	case BillboardType::NONE:
	default:
		break;
	}

	Mat44 orientation;
	orientation.SetIJK3D(billboardForward, billboardLeft, billboardUp);
	orientation.Orthonormalize_XFwd_YLeft_ZUp();

	Vec3 scaledLeft = orientation.GetJBasis3D() * billboardScale.x;
	Vec3 scaledUp = orientation.GetKBasis3D() * billboardScale.y;

	Mat44 billboardTransform;
	billboardTransform.SetIJKT3D(
		orientation.GetIBasis3D(),
		scaledLeft,
		scaledUp,
		billboardPosition);
	return billboardTransform;
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

//-----------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsLineSegment2D(Vec2 startPos, Vec2 fwdNormal, float maxDist,
	Vec2 lineStart, Vec2 lineEnd)
{
	RaycastResult2D missResult;
	missResult.m_didImpact = false;
	missResult.m_impactDist = maxDist;
	missResult.m_impactPos = startPos + (fwdNormal * maxDist);

	Vec2 const segmentDisplacement = lineEnd - lineStart;
	if (segmentDisplacement.GetLengthSquared() == 0.f) {
		return missResult;
	}

	Vec2 const iBasis = fwdNormal;
	Vec2 const jBasis = iBasis.GetRotatedBy90Degrees();
	Vec2 const rayToLineStart = lineStart - startPos;
	Vec2 const rayToLineEnd = lineEnd - startPos;

	float const lineStartJ = DotProduct2D(rayToLineStart, jBasis);
	float const lineEndJ = DotProduct2D(rayToLineEnd, jBasis);
	if ((lineStartJ * lineEndJ) >= 0.f) {
		return missResult;
	}

	float const lineStartI = DotProduct2D(rayToLineStart, iBasis);
	float const lineEndI = DotProduct2D(rayToLineEnd, iBasis);
	if ((lineStartI <= 0.f) && (lineEndI <= 0.f)) {
		return missResult;
	}
	if ((lineStartI >= maxDist) && (lineEndI >= maxDist)) {
		return missResult;
	}

	float const lineFraction = lineStartJ / (lineStartJ - lineEndJ);
	float const impactDist = lineStartI + lineFraction * (lineEndI - lineStartI);
	if ((impactDist <= 0.f) || (impactDist >= maxDist)) {
		return missResult;
	}

	RaycastResult2D hitResult;
	hitResult.m_didImpact = true;
	hitResult.m_impactDist = impactDist;
	hitResult.m_impactPos = startPos + (fwdNormal * impactDist);
	hitResult.m_rayStartPos = startPos;
	hitResult.m_rayFwdNormal = fwdNormal;
	hitResult.m_rayMaxLength = maxDist;

	Vec2 impactNormal = segmentDisplacement.GetRotatedBy90Degrees().GetNormalized();
	if (DotProduct2D(impactNormal, fwdNormal) > 0.f) {
		impactNormal = -impactNormal;
	}
	hitResult.m_impactNormal = impactNormal;

	return hitResult;
}

//-----------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsAABB2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2 const& aabb)
{
	RaycastResult2D missResult;
	missResult.m_didImpact = false;
	missResult.m_impactDist = maxDist;
	missResult.m_impactPos = startPos + (fwdNormal * maxDist);

	if (IsPointInsideAABB2D(startPos, aabb)) {
		RaycastResult2D immediateHit;
		immediateHit.m_didImpact = true;
		immediateHit.m_impactDist = 0.f;
		immediateHit.m_impactPos = startPos;
		immediateHit.m_impactNormal = -fwdNormal;
		immediateHit.m_rayStartPos = startPos;
		immediateHit.m_rayFwdNormal = fwdNormal;
		immediateHit.m_rayMaxLength = maxDist;
		return immediateHit;
	}

	float xEntry = -INFINITY;
	float xExit = INFINITY;
	if (fwdNormal.x == 0.f) {
		if ((startPos.x <= aabb.m_mins.x) || (startPos.x >= aabb.m_maxs.x)) {
			return missResult;
		}
	}
	else {
		float xToMin = (aabb.m_mins.x - startPos.x) / fwdNormal.x;
		float xToMax = (aabb.m_maxs.x - startPos.x) / fwdNormal.x;
		xEntry = xToMin < xToMax ? xToMin : xToMax;
		xExit = xToMin > xToMax ? xToMin : xToMax;
	}

	float yEntry = -INFINITY;
	float yExit = INFINITY;
	if (fwdNormal.y == 0.f) {
		if ((startPos.y <= aabb.m_mins.y) || (startPos.y >= aabb.m_maxs.y)) {
			return missResult;
		}
	}
	else {
		float yToMin = (aabb.m_mins.y - startPos.y) / fwdNormal.y;
		float yToMax = (aabb.m_maxs.y - startPos.y) / fwdNormal.y;
		yEntry = yToMin < yToMax ? yToMin : yToMax;
		yExit = yToMin > yToMax ? yToMin : yToMax;
	}

	float const impactDist = xEntry > yEntry ? xEntry : yEntry;
	float const exitDist = xExit < yExit ? xExit : yExit;
	if (impactDist >= exitDist) {
		return missResult;
	}
	if ((impactDist <= 0.f) || (impactDist > maxDist)) {
		return missResult;
	}

	RaycastResult2D hitResult;
	hitResult.m_didImpact = true;
	hitResult.m_impactDist = impactDist;
	hitResult.m_impactPos = startPos + (fwdNormal * impactDist);
	hitResult.m_rayStartPos = startPos;
	hitResult.m_rayFwdNormal = fwdNormal;
	hitResult.m_rayMaxLength = maxDist;

	Vec2 impactNormal = Vec2::ZERO;
	if (xEntry > yEntry) {
		impactNormal = fwdNormal.x > 0.f ? Vec2(-1.f, 0.f) : Vec2(1.f, 0.f);
	}
	else if (yEntry > xEntry) {
		impactNormal = fwdNormal.y > 0.f ? Vec2(0.f, -1.f) : Vec2(0.f, 1.f);
	}
	else {
		if (fabsf(fwdNormal.x) >= fabsf(fwdNormal.y)) {
			impactNormal = fwdNormal.x > 0.f ? Vec2(-1.f, 0.f) : Vec2(1.f, 0.f);
		}
		else {
			impactNormal = fwdNormal.y > 0.f ? Vec2(0.f, -1.f) : Vec2(0.f, 1.f);
		}
	}
	hitResult.m_impactNormal = impactNormal;

	return hitResult;
}

RaycastResult3D RaycastVsSphere3D(Vec3 startPos, Vec3 fwdNormal, float maxDist,
	Vec3 sphereCenter, float sphereRadius)
{
	RaycastResult3D missResult;
	missResult.m_didImpact = false;
	missResult.m_impactDist = maxDist;
	missResult.m_impactPos = startPos + (fwdNormal * maxDist);
	missResult.m_rayStartPos = startPos;
	missResult.m_rayFwdNormal = fwdNormal;
	missResult.m_rayMaxLength = maxDist;

	if (IsPointInsideSphere3D(startPos, sphereCenter, sphereRadius)) {
		RaycastResult3D immediateHit = missResult;
		immediateHit.m_didImpact = true;
		immediateHit.m_impactDist = 0.f;
		immediateHit.m_impactPos = startPos;
		immediateHit.m_impactNormal = -GetNormalizedOrFallback(fwdNormal, Vec3(1.f, 0.f, 0.f));
		return immediateHit;
	}

	Vec3 toCenter = sphereCenter - startPos;
	float projectionLength = DotProduct3D(toCenter, fwdNormal);
	float toCenterLengthSquared = toCenter.GetLengthSquared();
	float closestDistSquared = toCenterLengthSquared - projectionLength * projectionLength;
	float radiusSquared = sphereRadius * sphereRadius;
	if (closestDistSquared > radiusSquared) {
		return missResult;
	}

	float hitOffset = sqrtf(radiusSquared - closestDistSquared);
	float impactDist = projectionLength - hitOffset;
	if (impactDist < 0.f || impactDist > maxDist) {
		return missResult;
	}

	RaycastResult3D hitResult = missResult;
	hitResult.m_didImpact = true;
	hitResult.m_impactDist = impactDist;
	hitResult.m_impactPos = startPos + (fwdNormal * impactDist);
	hitResult.m_impactNormal = (hitResult.m_impactPos - sphereCenter).GetNormalized();
	return hitResult;
}

RaycastResult3D RaycastVsAABB3D(Vec3 startPos, Vec3 fwdNormal, float maxDist,
	AABB3 const& aabb)
{
	RaycastResult3D missResult;
	missResult.m_didImpact = false;
	missResult.m_impactDist = maxDist;
	missResult.m_impactPos = startPos + (fwdNormal * maxDist);
	missResult.m_rayStartPos = startPos;
	missResult.m_rayFwdNormal = fwdNormal;
	missResult.m_rayMaxLength = maxDist;

	if (IsPointInsideAABB3D(startPos, aabb)) {
		RaycastResult3D immediateHit = missResult;
		immediateHit.m_didImpact = true;
		immediateHit.m_impactDist = 0.f;
		immediateHit.m_impactPos = startPos;
		immediateHit.m_impactNormal = -GetNormalizedOrFallback(fwdNormal, Vec3(1.f, 0.f, 0.f));
		return immediateHit;
	}

	float tMin = 0.f;
	float tMax = maxDist;
	Vec3 impactNormal;

	for (int axisIndex = 0; axisIndex < 3; ++axisIndex) {
		float axisOrigin = axisIndex == 0 ? startPos.x : (axisIndex == 1 ? startPos.y : startPos.z);
		float axisDirection = axisIndex == 0 ? fwdNormal.x : (axisIndex == 1 ? fwdNormal.y : fwdNormal.z);
		float axisMin = axisIndex == 0 ? aabb.m_mins.x : (axisIndex == 1 ? aabb.m_mins.y : aabb.m_mins.z);
		float axisMax = axisIndex == 0 ? aabb.m_maxs.x : (axisIndex == 1 ? aabb.m_maxs.y : aabb.m_maxs.z);

		if (fabsf(axisDirection) < 1e-6f) {
			if (axisOrigin < axisMin || axisOrigin > axisMax) {
				return missResult;
			}
			continue;
		}

		float t1 = (axisMin - axisOrigin) / axisDirection;
		float t2 = (axisMax - axisOrigin) / axisDirection;
		float tNear = t1 < t2 ? t1 : t2;
		float tFar = t1 > t2 ? t1 : t2;

		Vec3 nearNormal;
		if (axisIndex == 0) nearNormal = axisDirection > 0.f ? Vec3(-1.f, 0.f, 0.f) : Vec3(1.f, 0.f, 0.f);
		if (axisIndex == 1) nearNormal = axisDirection > 0.f ? Vec3(0.f, -1.f, 0.f) : Vec3(0.f, 1.f, 0.f);
		if (axisIndex == 2) nearNormal = axisDirection > 0.f ? Vec3(0.f, 0.f, -1.f) : Vec3(0.f, 0.f, 1.f);

		if (tNear > tMin) {
			tMin = tNear;
			impactNormal = nearNormal;
		}
		if (tFar < tMax) {
			tMax = tFar;
		}

		if (tMin > tMax) {
			return missResult;
		}
	}

	if (tMin < 0.f || tMin > maxDist) {
		return missResult;
	}

	RaycastResult3D hitResult = missResult;
	hitResult.m_didImpact = true;
	hitResult.m_impactDist = tMin;
	hitResult.m_impactPos = startPos + (fwdNormal * tMin);
	hitResult.m_impactNormal = impactNormal;
	return hitResult;
}

RaycastResult3D RaycastVsZCylinder3D(Vec3 startPos, Vec3 fwdNormal, float maxDist,
	Vec2 cylinderCenterXY, float cylinderRadius, float cylinderMinZ, float cylinderMaxZ)
{
	RaycastResult3D missResult;
	missResult.m_didImpact = false;
	missResult.m_impactDist = maxDist;
	missResult.m_impactPos = startPos + (fwdNormal * maxDist);
	missResult.m_rayStartPos = startPos;
	missResult.m_rayFwdNormal = fwdNormal;
	missResult.m_rayMaxLength = maxDist;

	if (IsPointInsideZCylinder3D(startPos, cylinderCenterXY, cylinderRadius, cylinderMinZ, cylinderMaxZ)) {
		RaycastResult3D immediateHit = missResult;
		immediateHit.m_didImpact = true;
		immediateHit.m_impactDist = 0.f;
		immediateHit.m_impactPos = startPos;
		immediateHit.m_impactNormal = -GetNormalizedOrFallback(fwdNormal, Vec3(1.f, 0.f, 0.f));
		return immediateHit;
	}

	bool foundHit = false;
	float bestImpactDist = maxDist;
	Vec3 bestImpactPos;
	Vec3 bestImpactNormal;

	Vec2 startToCenter(startPos.x - cylinderCenterXY.x, startPos.y - cylinderCenterXY.y);
	float a = fwdNormal.x * fwdNormal.x + fwdNormal.y * fwdNormal.y;
	float b = 2.f * (startToCenter.x * fwdNormal.x + startToCenter.y * fwdNormal.y);
	float c = startToCenter.GetLengthSquared() - cylinderRadius * cylinderRadius;

	if (a > 0.f) {
		float discriminant = b * b - 4.f * a * c;
		if (discriminant >= 0.f) {
			float sqrtDiscriminant = sqrtf(discriminant);
			float inv2a = 0.5f / a;
			float t0 = (-b - sqrtDiscriminant) * inv2a;
			float t1 = (-b + sqrtDiscriminant) * inv2a;
			if (t0 > t1) {
				float temp = t0;
				t0 = t1;
				t1 = temp;
			}

			float sideCandidates[2] = { t0, t1 };
			for (float sideT : sideCandidates) {
				if (sideT < 0.f || sideT > bestImpactDist || sideT > maxDist) {
					continue;
				}

				float hitZ = startPos.z + sideT * fwdNormal.z;
				if (hitZ < cylinderMinZ || hitZ > cylinderMaxZ) {
					continue;
				}

				foundHit = true;
				bestImpactDist = sideT;
				bestImpactPos = startPos + (fwdNormal * sideT);
				bestImpactNormal = Vec3(bestImpactPos.x - cylinderCenterXY.x, bestImpactPos.y - cylinderCenterXY.y, 0.f).GetNormalized();
			}
		}
	}

	if (fabsf(fwdNormal.z) > 1e-6f) {
		float capZ[2] = { cylinderMinZ, cylinderMaxZ };
		for (float currentCapZ : capZ) {
			float capT = (currentCapZ - startPos.z) / fwdNormal.z;
			if (capT < 0.f || capT > bestImpactDist || capT > maxDist) {
				continue;
			}

			Vec3 capHitPos = startPos + (fwdNormal * capT);
			Vec2 capHitXY(capHitPos.x, capHitPos.y);
			if (GetDistanceSquared2D(capHitXY, cylinderCenterXY) > cylinderRadius * cylinderRadius) {
				continue;
			}

			foundHit = true;
			bestImpactDist = capT;
			bestImpactPos = capHitPos;
			bestImpactNormal = currentCapZ == cylinderMaxZ ? Vec3(0.f, 0.f, 1.f) : Vec3(0.f, 0.f, -1.f);
		}
	}

	if (!foundHit) {
		return missResult;
	}

	RaycastResult3D hitResult = missResult;
	hitResult.m_didImpact = true;
	hitResult.m_impactDist = bestImpactDist;
	hitResult.m_impactPos = bestImpactPos;
	hitResult.m_impactNormal = bestImpactNormal;
	return hitResult;
}

