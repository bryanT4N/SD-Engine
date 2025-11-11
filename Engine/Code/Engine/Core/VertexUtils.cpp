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

//-----------------------------------------------------------------------------------------------
void AddvertsForDisc2D(std::vector<Vertex>& verts, Vec2 disCenter, float discRadius, Rgba8 color)
{
	const int	numSlices	= 32;
	const float deltaTheta	= 360.f / numSlices;

	Vec3 centerPos = Vec3(disCenter);

	for (int i = 0; i < numSlices; ++i)
	{
		float startDeg = i * deltaTheta;
		float endDeg = (i + 1) * deltaTheta;

		Vec2 startPos2D = disCenter + discRadius * Vec2(CosDegrees(startDeg), SinDegrees(startDeg));
		Vec2 endPos2D	= disCenter + discRadius * Vec2(CosDegrees(endDeg), SinDegrees(endDeg));

		Vec2 startUV = Vec2(0.5f + 0.5f * CosDegrees(startDeg), 0.5f + 0.5f * SinDegrees(startDeg));
		Vec2 endUV = Vec2(0.5f + 0.5f * CosDegrees(endDeg), 0.5f + 0.5f * SinDegrees(endDeg));

		verts.push_back(Vertex(centerPos, color, Vec2(0.5f, 0.5f)));
		verts.push_back(Vertex(Vec3(startPos2D), color, startUV));
		verts.push_back(Vertex(Vec3(endPos2D), color, endUV));
	}
}

void AddvertsForRing2D(std::vector<Vertex>& verts, Vec2 ringCenter, float ringRadius, float thickness, Rgba8 color)
{
	const int numSlices = 32;
	const float deltaTheta = 360.f / numSlices;

	float innerRadius = ringRadius - thickness * 0.5f;
	float outerRadius = ringRadius + thickness * 0.5f;

	for (int i = 0; i < numSlices; ++i)
	{
		float startDeg	= i * deltaTheta;
		float endDeg	= (i + 1) * deltaTheta;

		Vec2 innerStartPos	= ringCenter + innerRadius * Vec2(CosDegrees(startDeg), SinDegrees(startDeg));
		Vec2 innerEndPos	= ringCenter + innerRadius * Vec2(CosDegrees(endDeg), SinDegrees(endDeg));
		Vec2 outerStartPos	= ringCenter + outerRadius * Vec2(CosDegrees(startDeg), SinDegrees(startDeg));
		Vec2 outerEndPos	= ringCenter + outerRadius * Vec2(CosDegrees(endDeg), SinDegrees(endDeg));

		verts.push_back(Vertex(Vec3(innerStartPos), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex(Vec3(outerStartPos), color, Vec2(1.f, 0.f))); verts.push_back(Vertex(Vec3(innerEndPos), color, Vec2(0.f, 1.f)));
		verts.push_back(Vertex(Vec3(innerEndPos), color, Vec2(0.f, 1.f))); verts.push_back(Vertex(Vec3(outerStartPos), color, Vec2(1.f, 0.f)));
		verts.push_back(Vertex(Vec3(outerEndPos), color, Vec2(1.f, 1.f)));
	}
}

void AddVertsForAABB2(std::vector<Vertex> &verts, const AABB2& alignedBox, Rgba8 color)
{
	Vertex v0 = Vertex(Vec3(alignedBox.m_mins.x, alignedBox.m_mins.y, 0.f), color, Vec2(0.f, 0.f));
	Vertex v1 = Vertex(Vec3(alignedBox.m_mins.x, alignedBox.m_maxs.y, 0.f), color, Vec2(0.f, 1.f));
	Vertex v2 = Vertex(Vec3(alignedBox.m_maxs.x, alignedBox.m_mins.y, 0.f), color, Vec2(1.f, 0.f));
	Vertex v3 = Vertex(Vec3(alignedBox.m_maxs.x, alignedBox.m_maxs.y, 0.f), color, Vec2(1.f, 1.f));

	verts.push_back(v0);
	verts.push_back(v2); verts.push_back(v1); 
	verts.push_back(v1); verts.push_back(v2);
	verts.push_back(v3);
}

void AddVertsForAABB2D(std::vector<Vertex>& verts, const AABB2& alignedBox, Rgba8 color)
{
	Vertex v0 = Vertex(Vec3(alignedBox.m_mins.x, alignedBox.m_mins.y, 0.f), color, Vec2(0.f, 0.f));
	Vertex v1 = Vertex(Vec3(alignedBox.m_mins.x, alignedBox.m_maxs.y, 0.f), color, Vec2(0.f, 1.f));
	Vertex v2 = Vertex(Vec3(alignedBox.m_maxs.x, alignedBox.m_mins.y, 0.f), color, Vec2(1.f, 0.f));
	Vertex v3 = Vertex(Vec3(alignedBox.m_maxs.x, alignedBox.m_maxs.y, 0.f), color, Vec2(1.f, 1.f));

	verts.push_back(v0);
	verts.push_back(v2); verts.push_back(v1);
	verts.push_back(v1); verts.push_back(v2);
	verts.push_back(v3);
}

void AddVertsForAABB2D(std::vector<Vertex>& verts, const AABB2& alignedBox, Rgba8 color, AABB2 UVs)
{
	Vertex v0 = Vertex(Vec3(alignedBox.m_mins.x, alignedBox.m_mins.y, 0.f), color, Vec2(UVs.m_mins.x, UVs.m_mins.y));
	Vertex v1 = Vertex(Vec3(alignedBox.m_mins.x, alignedBox.m_maxs.y, 0.f), color, Vec2(UVs.m_mins.x, UVs.m_maxs.y));
	Vertex v2 = Vertex(Vec3(alignedBox.m_maxs.x, alignedBox.m_mins.y, 0.f), color, Vec2(UVs.m_maxs.x, UVs.m_mins.y));
	Vertex v3 = Vertex(Vec3(alignedBox.m_maxs.x, alignedBox.m_maxs.y, 0.f), color, Vec2(UVs.m_maxs.x, UVs.m_maxs.y));

	verts.push_back(v0);
	verts.push_back(v2); verts.push_back(v1);
	verts.push_back(v1); verts.push_back(v2);
	verts.push_back(v3);
}

void AddvertsForOBB2D(std::vector<Vertex>& verts, const OBB2& orientedBox, Rgba8 color)
{
	Vec2 iBasis = orientedBox.m_iBasisNormal.GetNormalized();
	Vec2 jBasis = iBasis.GetRotatedBy90Degrees();
	Vec2 translation = orientedBox.m_center;
	Vec2 halfDim = orientedBox.m_halfDimensions;

	Vec2 bl(-halfDim.x, -halfDim.y); Vec2 br(halfDim.x, -halfDim.y);
	Vec2 tl(-halfDim.x, halfDim.y); Vec2 tr(halfDim.x, halfDim.y);

	TransformPosition2D(bl, iBasis, jBasis, translation);
	TransformPosition2D(br, iBasis, jBasis, translation);
	TransformPosition2D(tl, iBasis, jBasis, translation);
	TransformPosition2D(tr, iBasis, jBasis, translation);

	verts.push_back(Vertex(Vec3(bl), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex(Vec3(br), color, Vec2(1.f, 0.f))); verts.push_back(Vertex(Vec3(tl), color, Vec2(0.f, 1.f)));
	verts.push_back(Vertex(Vec3(tl), color, Vec2(0.f, 1.f))); verts.push_back(Vertex(Vec3(br), color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex(Vec3(tr), color, Vec2(1.f, 1.f)));
}

void AddVertsForCapsule2D(std::vector<Vertex>& verts, Vec2 boneStart, Vec2 boneEnd, float radius, Rgba8 color)
{
	OBB2 obb2;
	obb2.m_center = 0.5f * (boneStart + boneEnd);
	obb2.m_iBasisNormal = (boneEnd - boneStart).GetNormalized();
	obb2.m_halfDimensions = Vec2(0.5f * (boneEnd - boneStart).GetLength(), radius);
	AddvertsForOBB2D(verts, obb2, color);

	Vec2 dir = obb2.m_iBasisNormal;
	AddvertsForSector(verts, boneStart, radius, -dir, 180.f, color);
	AddvertsForSector(verts, boneEnd, radius, dir, 180.f, color);
}

void AddvertsForTriangle2D(std::vector<Vertex>& verts, Vec2 ccw0, Vec2 ccw1, Vec2 ccw2, Rgba8 color)
{
	verts.push_back(Vertex(Vec3(ccw0), color, Vec2::ZERO));
	verts.push_back(Vertex(Vec3(ccw1), color, Vec2::ZERO));
	verts.push_back(Vertex(Vec3(ccw2), color, Vec2::ZERO));
}

void AddVertsForLineSegment2D(std::vector<Vertex>& verts, Vec2 start, Vec2 end, const float& thickness, Rgba8 color)
{
	// Compute (half-thickness-long) forward and left displacement
	Vec2 forwardNormal = (end - start).GetNormalized();
	Vec2 forwardStep = forwardNormal * (thickness * 0.5f);
	Vec2 leftStep = forwardStep.GetRotatedBy90Degrees();

	// Compute four corner positions
	Vec2 endLeft = end + forwardStep + leftStep;
	Vec2 endRight = end + forwardStep - leftStep;
	Vec2 startLeft = start - forwardStep + leftStep;
	Vec2 startRight = start - forwardStep - leftStep;

	verts.push_back(Vertex(Vec3(startRight), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex(Vec3(endRight), color, Vec2(1.f, 0.f))); verts.push_back(Vertex(Vec3(startLeft), color, Vec2(0.f, 1.f)));
	verts.push_back(Vertex(Vec3(startLeft), color, Vec2(0.f, 1.f))); verts.push_back(Vertex(Vec3(endRight), color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex(Vec3(endLeft), color, Vec2(1.f, 1.f)));
}

void AddvertsForSector(std::vector<Vertex>& verts, Vec2 origin, float radius, Vec2 direction, float angleDegrees, Rgba8 color)
{
	const int	numSlices	= 32;

	Vec2		iBasis		= direction.GetNormalized();
	Vec2		jBasis		= iBasis.GetRotatedBy90Degrees();

	float		halfAngle	= angleDegrees * 0.5f;
	float		deltaTheta	= angleDegrees / numSlices;

	for (int i = 0; i < numSlices; ++i)
	{
		float thetaStart	= -halfAngle + i * deltaTheta;
		float thetaEnd		= -halfAngle + (i + 1) * deltaTheta;

		Vec2 start		= Vec2(CosDegrees(thetaStart), SinDegrees(thetaStart)) * radius;
		Vec2 end		= Vec2(CosDegrees(thetaEnd), SinDegrees(thetaEnd)) * radius;

		TransformPosition2D(start, iBasis, jBasis, origin);
		TransformPosition2D(end, iBasis, jBasis, origin);

		verts.push_back(Vertex(Vec3(origin), color, Vec2(0.5f, 0.5f)));
		verts.push_back(Vertex(Vec3(start), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex(Vec3(end), color, Vec2(1.f, 0.f)));
	}
}

