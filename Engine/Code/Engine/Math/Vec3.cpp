#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
//#include "Engine/Core/EngineCommon.hpp"

#define _USE_MATH_DEFINES
#include <cmath>


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( Vec3 const& copy )
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
{
}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{
}


Vec3::Vec3(Vec2 const& vecXY)
	: x(vecXY.x)
	, y(vecXY.y)
	, z(0.f)
{
}


Vec3 const Vec3::MakeFromPolarRadians(
	float pitchRadians,
	float yawRadians,
	float length)
{
	float cosPitch = cosf(pitchRadians);
	return Vec3(
		length * cosf(yawRadians) * cosPitch,
		length * sinf(yawRadians) * cosPitch,
		-length * sinf(pitchRadians));
}


Vec3 const Vec3::MakeFromPolarDegrees(
	float pitchDegrees,
	float yawDegrees,
	float length)
{
	return MakeFromPolarRadians(
		ConvertDegreesToRadians(pitchDegrees),
		ConvertDegreesToRadians(yawDegrees),
		length);
}

float Vec3::GetLength() const
{
	return sqrtf(x * x + y * y + z * z);
}

float Vec3::GetLengthXY() const
{
	return sqrtf(x * x + y * y);
}

float Vec3::GetLengthSquared() const
{
	return (x * x + y * y + z * z);
}

float Vec3::GetLengthXYSquared() const
{
	return (x * x + y * y);
}

float Vec3::GetOrientationAboutZDegrees() const
{
	return Vec2(x, y).GetOrientationDegrees();
}

float Vec3::GetOrientationAboutZRadians() const
{
	return Vec2(x, y).GetOrientationRadians();
}

Vec3 const Vec3::GetRotatedAboutZDegrees(float rotationDegreesAboutZ) const
{
	Vec2 RotatedXY = Vec2(x, y).GetRotatedByDegrees(rotationDegreesAboutZ);
	return Vec3(RotatedXY.x, RotatedXY.y, z);
}

Vec3 const Vec3::GetRotatedAboutZRadians(float rotationRadiansAboutZ) const
{
	Vec2 RotatedXY = Vec2(x, y).GetRotatedByRadians(rotationRadiansAboutZ);
	return Vec3(RotatedXY.x, RotatedXY.y, z);
}

Vec3 const Vec3::GetClampedToMaxLength(float maxLength) const
{
	if( GetLengthSquared() > maxLength * maxLength ) { return GetNormalized() * maxLength; }
	return *this;
}

Vec3 const Vec3::GetNormalized() const
{
	float length = GetLength();
	if( length == 0.f ) {
		return Vec3(0.f, 0.f, 0.f);
	}

	return Vec3(x / length, y / length, z / length);
}

//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator + ( Vec3 const& vecToAdd ) const
{
	return Vec3(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator-( Vec3 const& vecToSubtract ) const
{
	return Vec3(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z);
}


//------------------------------------------------------------------------------------------------
Vec3 const Vec3::operator-() const
{
	return Vec3(-x, -y, -z);
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator*( float uniformScale ) const
{
	return Vec3(uniformScale * x, uniformScale * y, uniformScale * z);
}


//------------------------------------------------------------------------------------------------
Vec3 const Vec3::operator*( Vec3 const& vecToMultiply ) const
{
	return Vec3(x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z);
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator/( float inverseScale ) const
{
	return Vec3(x / inverseScale, y / inverseScale, z / inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( Vec3 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( Vec3 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=( Vec3 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
Vec3 const operator*( float uniformScale, Vec3 const& vecToScale )
{
	return Vec3(uniformScale*vecToScale.x, uniformScale * vecToScale.y, uniformScale * vecToScale.z);
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( Vec3 const& compare ) const
{
	return x == compare.x && y == compare.y && z == compare.z;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( Vec3 const& compare ) const
{
	return !(x == compare.x && y == compare.y && z == compare.z);
}


