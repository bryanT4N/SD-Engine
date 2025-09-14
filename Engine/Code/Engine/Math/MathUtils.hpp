#pragma once
// #include "Engine/Math/Vec2.hpp"
// #include "Engine/Math/Vec3.hpp"

//-----------------------------------------------------------------------------------------------
// Forward type declarations
struct Vec2;
struct Vec3;

//-----------------------------------------------------------------------------------------------
// Angle utilities
float ConvertDegreesToRadians(float degrees); 
float ConvertRadiansToDegrees(float radians); 
float CosDegrees(float degrees); 
float SinDegrees(float degrees); 
float Atan2Degrees(float y, float x);

//-----------------------------------------------------------------------------------------------
// Basic 2D & 3D utilities
float GetDistance2D(Vec2 const& positionA, Vec2 const& positionB); 
float GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB); 
float GetDistance3D(Vec3 const& positionA, Vec3 const& positionB); 
float GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB); 
float GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB);
float GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB);

//-----------------------------------------------------------------------------------------------
// Geometric query utilities
bool DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB); 
bool DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB);

//-----------------------------------------------------------------------------------------------
// Transform utilities
void TransformPosition2D( Vec2& posToTransform, float uniformscale, float rotationDegrees, Vec2 const& translation );
void TransformPositionXY3D( Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation );