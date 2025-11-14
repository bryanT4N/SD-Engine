#pragma once

#include <vector>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Core/Rgba8.hpp"

struct Vec2;
struct Vertex;

//-----------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D(int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);
void TransformVertexArrayXY3D(std::vector<Vertex> &verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);

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