#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

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

float GetShortestAngularDispDegrees(float startDegrees, float endDegrees)
{
	float d1 = fmod(startDegrees, 360.f),
		d2 = fmod(endDegrees, 360.f);
	
	d1 = d1 > 0.f ? d1 : d1 + 360.f;
	d2 = d2 > 0.f ? d2 : d2 + 360.f;

	float angularDispDegrees = d2 - d1;
	float angularDispDegreesAbs = abs(angularDispDegrees);

	return angularDispDegreesAbs < 180.f ? angularDispDegrees : copysign(360.f - angularDispDegreesAbs, - angularDispDegrees);
}

float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees)
{
	goalDegrees = currentDegrees + GetShortestAngularDispDegrees(currentDegrees, goalDegrees);
	float turnedDegrees = fmod(GetClamped(goalDegrees, currentDegrees - maxDeltaDegrees, currentDegrees + maxDeltaDegrees), 360.f);
	return turnedDegrees > 0 ? turnedDegrees : turnedDegrees + 360.f;
}

float DotProduct2D(Vec2 const& a, Vec2 const& b)
{
	return a.x * b.x + a.y * b.y;
}

float GetDistance2D(Vec2 const& positionA, Vec2 const& positionB)
{
	return (positionB - positionA).GetLength();
}

float GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB)
{
	return (positionB - positionA).GetLengthSquared();
}

float GetDistance3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return (positionB - positionA).GetLength();
}

float GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return (positionB - positionA).GetLengthSquared();
}

float GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return (positionB - positionA).GetLengthXY();
}

float GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return (positionB - positionA).GetLengthXYSquared();
}

bool DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB)
{
	return (radiusA + radiusB) * (radiusA + radiusB) > (centerA - centerB).GetLengthSquared();
}

bool DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB)
{
	return (radiusA + radiusB) * (radiusA + radiusB) > (centerA - centerB).GetLengthSquared();
}

void TransformPosition2D(Vec2& posToTransform, float uniformscale, float rotationDegrees, Vec2 const& translation)
{
	// Scale
	posToTransform = posToTransform * uniformscale;

	// Rotate
	posToTransform = posToTransform.GetRotatedByDegrees(rotationDegrees);

	// Translate
	posToTransform = posToTransform + translation;
}

void TransformPositionXY3D(Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation)
{
	// XY-Scale
	posToTransform = posToTransform * Vec3(xyScale, xyScale, 1);

	// Z-Rotate
	posToTransform = posToTransform.GetRotatedAboutZDegrees(zRotationDegrees);

	// XY-Translate
	posToTransform = posToTransform + Vec3(xyTranslation.x, xyTranslation.y, 0);
}

