#pragma once

#include <vector>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"

struct Vec2;
struct Vertex;

//-----------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D(int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);
void TransformVertexArrayXY3D(std::vector<Vertex> &verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);

void AddVertsForAABB2(std::vector<Vertex> &verts, AABB2 aabb, Rgba8 color);