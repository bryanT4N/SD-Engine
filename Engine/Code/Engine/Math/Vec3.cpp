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
	return Atan2Degrees(y, x);
}

float Vec3::GetOrientationAboutZRadians() const
{
	return atan2f(y, x);
}

Vec3 const Vec3::GetRotatedAboutZDegrees(float rotationDegreesAboutZ) const
{
	return Vec3(
		x * CosDegrees(rotationDegreesAboutZ) + y * (-SinDegrees(rotationDegreesAboutZ)), 
		x * SinDegrees(rotationDegreesAboutZ) + y * (CosDegrees(rotationDegreesAboutZ)), 
		z);
}

Vec3 const Vec3::GetRotatedAboutZRadians(float rotationRadiansAboutZ) const
{
	return Vec3(
		x * cosf(rotationRadiansAboutZ) + y * (-sinf(rotationRadiansAboutZ)),
		x * sinf(rotationRadiansAboutZ) + y * (cosf(rotationRadiansAboutZ)),
		z);
}

Vec3 const Vec3::GetClampedToMaxLength(float maxLength) const
{
	if( GetLengthSquared() > maxLength * maxLength ) { return GetNormalized() * maxLength; }
	return *this;
}

Vec3 const Vec3::GetNormalized() const
{
	float length = GetLength();
	return Vec3(x / length, y / length, y / length);
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


