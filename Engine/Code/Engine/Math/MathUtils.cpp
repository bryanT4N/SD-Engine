#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

#define _USE_MATH_DEFINES
#include <cmath>

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

void TransformPositionXY3D(Vec3& posToTransform, float xyScale, float zRotationbegrees, Vec2 const& xyTranslation)
{
	// XY-Scale
	posToTransform = posToTransform * Vec3(xyScale, xyScale, 1);

	// Z-Rotate
	posToTransform = posToTransform.GetRotatedAboutZDegrees(zRotationbegrees);

	// XY-Translate
	posToTransform = posToTransform + Vec3(xyTranslation.x, xyTranslation.y, 0);
}

