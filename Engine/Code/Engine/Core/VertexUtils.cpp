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

// 	for (int vertexIndex = 0; vertexIndex < numVerts; ++vertexIndex) {
// 		TransformPositionXY3D(verts[vertexIndex].m_position, uniformScaleXY, rotationDegreesAboutZ, translationXY);
// 	}
}

