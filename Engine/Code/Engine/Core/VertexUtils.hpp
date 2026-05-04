#pragma once

#include <vector>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"

struct Vec2;
struct Vec3;
struct Mat44;

//-----------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D(int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);
void TransformVertexArrayXY3D(std::vector<Vertex> &verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);
void AddVertsForQuad3D(std::vector<Vertex>& verts,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color = Rgba8::WHITE, const AABB2& UVs = AABB2(0.f, 0.f, 1.f, 1.f));
void AddVertsForQuad3D(std::vector<Vertex>& verts,
	std::vector<unsigned int>& indexes,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color = Rgba8::WHITE, const AABB2& UVs = AABB2(0.f, 0.f, 1.f, 1.f));
void AddVertsForRoundedQuad3D(std::vector<Vertex>& vertexes,
	const Vec3& topLeft, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight,
	const Rgba8& color = Rgba8::WHITE, const AABB2& UVs = AABB2(0.f, 0.f, 1.f, 1.f));
void AddVertsForAABB3D(std::vector<Vertex>& verts,
	AABB3 const& bounds,
	Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& UVs = AABB2(0.f, 0.f, 1.f, 1.f));
void AddVertsForSphere3D(std::vector<Vertex>& verts,
	Vec3 const& center,
	float radius,
	Rgba8 const& color = Rgba8::WHITE,
	AABB2 const& UVs = AABB2(0.f, 0.f, 1.f, 1.f),
	int numSlices = 32,
	int numStacks = 16);
void TransformVertexArray3D(std::vector<Vertex>& verts, Mat44 const& transform);
AABB2 GetVertexBounds2D(std::vector<Vertex> const& verts);
void AddVertsForCylinder3D(std::vector<Vertex>& verts,
	Vec3 const& start, Vec3 const& end, float radius,
	Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2(0.f, 0.f, 1.f, 1.f), int numSlices = 32);
void AddVertsForCone3D(std::vector<Vertex>& verts,
	Vec3 const& start, Vec3 const& end, float radius,
	Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2(0.f, 0.f, 1.f, 1.f), int numSlices = 32);
void AddVertsForArrow3D(std::vector<Vertex>& verts,
	Vec3 const& start, Vec3 const& end, float radius,
	Rgba8 const& color = Rgba8::WHITE, int numSlices = 32);

//-----------------------------------------------------------------------------------------------
void AddvertsForDisc2D(std::vector<Vertex> &verts, Vec2 disCenter, float discRadius, Rgba8 color);
void AddvertsForRing2D(std::vector<Vertex>& verts, Vec2 ringCenter, float ringRadius, float thickness, Rgba8 color); 
void AddVertsForAABB2(std::vector<Vertex>& verts, const AABB2& alignedBox, Rgba8 color);
void AddVertsForAABB2D(std::vector<Vertex>& verts, const AABB2& alignedBox, Rgba8 color);
void AddVertsForAABB2D(std::vector<Vertex>& verts, const AABB2& alignedBox, Rgba8 color, AABB2 UVs);
void AddvertsForOBB2D(std::vector<Vertex> &verts, const OBB2& orientedBox,Rgba8 color );
void AddVertsForCapsule2D(std::vector<Vertex> &verts, Vec2 boneStart, Vec2 boneEnd, float radius, Rgba8 color );
void AddvertsForTriangle2D(std::vector<Vertex> &verts, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2, Rgba8 color ); // counter-clockwise
void AddVertsForLineSegment2D(std::vector<Vertex>& verts, Vec2 start, Vec2 end, const float& thickness, Rgba8 color);
void AddvertsForSector(std::vector<Vertex>& verts, Vec2 origin, float radius, Vec2 direction, float angleDegrees, Rgba8 color);
void AddVertsForArrow2D( std::vector<Vertex>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 color);