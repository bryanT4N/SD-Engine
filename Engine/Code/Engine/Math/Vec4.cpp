#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"

#define _USE_MATH_DEFINES
#include <cmath>


//-----------------------------------------------------------------------------------------------
Vec4::Vec4( Vec4 const& copyFrom )
	: x( copyFrom.x )
	, y( copyFrom.y )
	, z( copyFrom.z )
	, w( copyFrom.w )
{
}


//-----------------------------------------------------------------------------------------------
Vec4::Vec4( float initialX, float initialY, float initialZ, float initialW )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
	, w( initialW )
{
}


//-----------------------------------------------------------------------------------------------
Vec4::Vec4( Vec3 const& vecXYZ, float initialW )
	: x( vecXYZ.x )
	, y( vecXYZ.y )
	, z( vecXYZ.z )
	, w( initialW )
{
}


//-----------------------------------------------------------------------------------------------
float Vec4::GetLength() const {
	return sqrtf( x * x + y * y + z * z + w * w );
}


//-----------------------------------------------------------------------------------------------
float Vec4::GetLengthXYZ() const {
	return sqrtf( x * x + y * y + z * z );
}


//-----------------------------------------------------------------------------------------------
float Vec4::GetLengthSquared() const {
	return x * x + y * y + z * z + w * w;
}


//-----------------------------------------------------------------------------------------------
float Vec4::GetLengthXYZSquared() const {
	return x * x + y * y + z * z;
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator+( Vec4 const& vecToAdd ) const {
	return Vec4( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w );
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator-( Vec4 const& vecToSubtract ) const {
	return Vec4( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w );
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator-() const {
	return Vec4( -x, -y, -z, -w );
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator*( float uniformScale ) const {
	return Vec4( uniformScale * x, uniformScale * y, uniformScale * z, uniformScale * w );
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator*( Vec4 const& vecToMultiply ) const {
	return Vec4( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z, w * vecToMultiply.w );
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator/( float inverseScale ) const {
	return Vec4( x / inverseScale, y / inverseScale, z / inverseScale, w / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator+=( Vec4 const& vecToAdd ) {
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator-=( Vec4 const& vecToSubtract ) {
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator*=( float uniformScale ) {
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator/=( float uniformDivisor ) {
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
	w /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator=( Vec4 const& copyFrom ) {
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}


//-----------------------------------------------------------------------------------------------
Vec4 const operator*( float uniformScale, Vec4 const& vecToScale ) {
	return Vec4( uniformScale * vecToScale.x,
	             uniformScale * vecToScale.y,
	             uniformScale * vecToScale.z,
	             uniformScale * vecToScale.w );
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator==( Vec4 const& compare ) const {
	return x == compare.x && y == compare.y && z == compare.z && w == compare.w;
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator!=( Vec4 const& compare ) const {
	return !( x == compare.x && y == compare.y && z == compare.z && w == compare.w );
}


