#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"

void TransformVertexArrayXY3D(int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY)
{
	Vec2 iBasis = Vec2::MakeFromPolarDegrees(rotationDegreesAboutZ, uniformScaleXY),
		jBasis = iBasis.GetRotatedBy90Degrees();

	for (int vertexIndex = 0; vertexIndex < numVerts; ++vertexIndex) {
		TransformPositionXY3D(verts[vertexIndex].m_position, iBasis, jBasis, translationXY);
	}

}

void TransformVertexArrayXY3D(std::vector<Vertex> &verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY)
{
	Vec2 iBasis = Vec2::MakeFromPolarDegrees(rotationDegreesAboutZ, uniformScaleXY),
		jBasis = iBasis.GetRotatedBy90Degrees();

	for (Vertex vert : verts) {
		TransformPositionXY3D(vert.m_position, iBasis, jBasis, translationXY);
	}
}

void AddVertsForAABB2(std::vector<Vertex> &verts, AABB2 aabb, Rgba8 color)
{
	Vertex vert1 = Vertex(Vec3(aabb.m_mins.x, aabb.m_mins.y, 0.f), color, Vec2(0.f, 0.f));
	Vertex vert2 = Vertex(Vec3(aabb.m_mins.x, aabb.m_maxs.y, 0.f), color, Vec2(0.f, 1.f));
	Vertex vert3 = Vertex(Vec3(aabb.m_maxs.x, aabb.m_mins.y, 0.f), color, Vec2(1.f, 0.f));
	Vertex vert4 = Vertex(Vec3(aabb.m_maxs.x, aabb.m_maxs.y, 0.f), color, Vec2(1.f, 1.f));

	verts.push_back(vert1);
	verts.push_back(vert2); verts.push_back(vert3);
	verts.push_back(vert2); verts.push_back(vert3);
	verts.push_back(vert4);
}

