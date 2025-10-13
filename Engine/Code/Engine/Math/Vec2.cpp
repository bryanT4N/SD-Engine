#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
//#include "Engine/Core/EngineCommon.hpp"

#define _USE_MATH_DEFINES
#include <cmath>


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( Vec2 const& copy )
	: x(copy.x)
	, y(copy.y)
{
}

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x(initialX)
	, y(initialY)
{
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarDegrees(float orientationDegrees, float length)
{
	return Vec2(length * CosDegrees(orientationDegrees), length * SinDegrees(orientationDegrees));
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarRadians(float orientationRadians, float length)
{
	return Vec2(length * cosf(orientationRadians), length * sinf(orientationRadians));
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetLength() const
{
	return sqrtf(x * x + y * y);
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetLengthSquared() const
{
	return (x * x + y * y);
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetOrientationDegrees() const
{
	return Atan2Degrees(y, x);
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetOrientationRadians() const
{
	return atan2f(y, x);
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedBy90Degrees() const
{
	return Vec2(-y, x);
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedByMinus90Degrees() const
{
	return Vec2(y, -x);
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedByDegrees(float rotationDegrees)
{
	return Vec2::MakeFromPolarDegrees(GetOrientationDegrees() + rotationDegrees, GetLength());
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedByRadians(float rotationRadians)
{
	return Vec2::MakeFromPolarRadians(GetOrientationRadians() + rotationRadians, GetLength());
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetClamped(float maxLength) const
{
	if( GetLengthSquared() > maxLength * maxLength ) { return GetNormalized() * maxLength; }
	return *this;
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetNormalized() const
{
	float length = GetLength();
	return Vec2(x / length, y / length);
}

Vec2 const Vec2::GetReflected(Vec2 const& normalOfSurfaceToReflectOffOf) const
{
	// Vec2 decompositedPerpendicular	= GetProjectedVector2D(*this, normalOfSurfaceToReflectOffOf);
	Vec2 decompositedPerpendicular	= normalOfSurfaceToReflectOffOf * DotProduct2D(*this, normalOfSurfaceToReflectOffOf);
	return *this - 2.0f * decompositedPerpendicular;
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetOrientationDegrees(float newOrientationDegrees)
{
	float length = GetLength();
	x = length * CosDegrees(newOrientationDegrees);
	y = length * SinDegrees(newOrientationDegrees);
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetOrientationRadians(float newOrientationRadians)
{
	float length = GetLength();
	x = length * cosf(newOrientationRadians);
	y = length * sinf(newOrientationRadians);
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarDegrees(float newOrientationDegrees, float newLength)
{
	x = newLength * CosDegrees(newOrientationDegrees);
	y = newLength * SinDegrees(newOrientationDegrees);
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarRadians(float newOrientationRadians, float newLength)
{
	x = newLength * cosf(newOrientationRadians);
	y = newLength * sinf(newOrientationRadians);
}

//-----------------------------------------------------------------------------------------------
void Vec2::Rotate90Degrees()
{
	Vec2 previous = Vec2(x, y);
	x = - previous.y;
	y = previous.x;
}

//-----------------------------------------------------------------------------------------------
void Vec2::RotateMinus90Degrees()
{
	Vec2 previous = Vec2(x, y);
	x = previous.y;
	y = - previous.x;
}

//-----------------------------------------------------------------------------------------------
void Vec2::RotateDegrees(float rotationDegrees)
{
	*this = GetRotatedByDegrees(rotationDegrees);
}

//-----------------------------------------------------------------------------------------------
void Vec2::RotateRadians(float rotationRadians)
{
	*this = GetRotatedByRadians(rotationRadians);
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetLength(float newlength)
{
	*this = GetNormalized() * newlength;
}

//-----------------------------------------------------------------------------------------------
void Vec2::ClampLength(float maxLength)
{
	if (GetLengthSquared() > maxLength * maxLength) { SetLength(maxLength); }
}

//-----------------------------------------------------------------------------------------------
void Vec2::Normalize()
{
	float length = GetLength();
	x /= length;
	y /= length;
}

//-----------------------------------------------------------------------------------------------
float Vec2::NormalizeAndGetPreviousLength()
{
	float length = GetLength();
	Normalize();
	return length;
}

void Vec2::Reflect(Vec2 const& normalOfSurfaceToReflectOffOf)
{
	Vec2 decompositedPerpendicular = normalOfSurfaceToReflectOffOf * DotProduct2D(*this, normalOfSurfaceToReflectOffOf);
	*this = *this - 2.0f * decompositedPerpendicular;
}

//-----------------------------------------------------------------------------------------------
Vec2 const operator*(float uniformScale, Vec2 const& vecToScale)
{
	return Vec2(uniformScale * vecToScale.x, uniformScale * vecToScale.y);
}

//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( Vec2 const& compare ) const
{
	return x == compare.x && y == compare.y;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( Vec2 const& compare ) const
{
	return !(x == compare.x && y == compare.y);
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator + ( Vec2 const& vecToAdd ) const
{
	return Vec2(x + vecToAdd.x, y + vecToAdd.y);
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator-( Vec2 const& vecToSubtract ) const
{
	return Vec2(x - vecToSubtract.x, y - vecToSubtract.y);
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator*( float uniformScale ) const
{
	return Vec2(uniformScale*x, uniformScale*y);
}


//------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator-() const
{
	return Vec2(-x, -y);
}


//------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator*( Vec2 const& vecToMultiply ) const
{
	return Vec2(x * vecToMultiply.x, y * vecToMultiply.y);
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator/( float inverseScale ) const
{
	return Vec2(x / inverseScale, y / inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( Vec2 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( Vec2 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( Vec2 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


