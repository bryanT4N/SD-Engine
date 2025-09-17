#pragma once

struct Vec2;
struct Vertex;

//-----------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D(int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);
