#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include <cmath>
#include <limits>

namespace
{
	int GetValidNumSlices(int numSlices)
	{
		return numSlices < 3 ? 3 : numSlices;
	}

	void GetSurfaceTangentsForAxis(Vec3 const& axisNormal, Vec3& outIBasis, Vec3& outJBasis)
	{
		Vec3 referenceUp = fabsf(axisNormal.z) < 0.999f ? Vec3(0.f, 0.f, 1.f) : Vec3(0.f, 1.f, 0.f);
		outIBasis = CrossProduct3D(referenceUp, axisNormal).GetNormalized();
		if (outIBasis.GetLengthSquared() == 0.f) {
			referenceUp = Vec3(1.f, 0.f, 0.f);
			outIBasis = CrossProduct3D(referenceUp, axisNormal).GetNormalized();
		}

		outJBasis = CrossProduct3D(axisNormal, outIBasis).GetNormalized();
	}

	Vec2 GetRadialUV(AABB2 const& UVs, float cosTheta, float sinTheta)
	{
		float u = RangeMap(cosTheta, -1.f, 1.f, UVs.m_mins.x, UVs.m_maxs.x);
		float v = RangeMap(sinTheta, -1.f, 1.f, UVs.m_mins.y, UVs.m_maxs.y);
		return Vec2(u, v);
	}
}

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

void AddVertsForQuad3D(std::vector<Vertex>& verts,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color, const AABB2& UVs)
{
	verts.push_back(Vertex(bottomLeft, color, Vec2(UVs.m_mins.x, UVs.m_mins.y)));
	verts.push_back(Vertex(bottomRight, color, Vec2(UVs.m_maxs.x, UVs.m_mins.y)));
	verts.push_back(Vertex(topRight, color, Vec2(UVs.m_maxs.x, UVs.m_maxs.y)));

	verts.push_back(Vertex(bottomLeft, color, Vec2(UVs.m_mins.x, UVs.m_mins.y)));
	verts.push_back(Vertex(topRight, color, Vec2(UVs.m_maxs.x, UVs.m_maxs.y)));
	verts.push_back(Vertex(topLeft, color, Vec2(UVs.m_mins.x, UVs.m_maxs.y)));
}

void AddVertsForQuad3D(std::vector<Vertex>& verts,
	std::vector<unsigned int>& indexes,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color, const AABB2& UVs)
{
	Vec3 edge1 = bottomRight - bottomLeft;
	Vec3 edge2 = topLeft - bottomLeft;
	Vec3 tangent = edge1.GetNormalized();
	if (tangent.GetLengthSquared() == 0.f) {
		tangent = Vec3(1.f, 0.f, 0.f);
	}

	Vec3 normal = CrossProduct3D(edge1, edge2).GetNormalized();
	if (normal.GetLengthSquared() == 0.f) {
		normal = Vec3(0.f, 0.f, 1.f);
	}

	Vec3 bitangent = CrossProduct3D(normal, tangent).GetNormalized();
	if (bitangent.GetLengthSquared() == 0.f) {
		bitangent = edge2.GetNormalized();
	}
	if (bitangent.GetLengthSquared() == 0.f) {
		bitangent = Vec3(0.f, 1.f, 0.f);
	}

	unsigned int startIndex = static_cast<unsigned int>(verts.size());
	verts.push_back(Vertex(
		bottomLeft,
		color,
		Vec2(UVs.m_mins.x, UVs.m_mins.y),
		tangent,
		bitangent,
		normal));
	verts.push_back(Vertex(
		bottomRight,
		color,
		Vec2(UVs.m_maxs.x, UVs.m_mins.y),
		tangent,
		bitangent,
		normal));
	verts.push_back(Vertex(
		topRight,
		color,
		Vec2(UVs.m_maxs.x, UVs.m_maxs.y),
		tangent,
		bitangent,
		normal));
	verts.push_back(Vertex(
		topLeft,
		color,
		Vec2(UVs.m_mins.x, UVs.m_maxs.y),
		tangent,
		bitangent,
		normal));

	indexes.push_back(startIndex + 0u);
	indexes.push_back(startIndex + 1u);
	indexes.push_back(startIndex + 2u);
	indexes.push_back(startIndex + 0u);
	indexes.push_back(startIndex + 2u);
	indexes.push_back(startIndex + 3u);
}


void AddVertsForRoundedQuad3D(std::vector<Vertex>& vertexes,
	const Vec3& topLeft, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight,
	const Rgba8& color, const AABB2& UVs)
{
	Vec3 tangent = (bottomRight - bottomLeft).GetNormalized();
	if (tangent.GetLengthSquared() == 0.f) {
		tangent = Vec3(1.f, 0.f, 0.f);
	}

	Vec3 bitangent = (topLeft - bottomLeft).GetNormalized();
	if (bitangent.GetLengthSquared() == 0.f) {
		bitangent = Vec3(0.f, 1.f, 0.f);
	}

	Vec3 trueNormal = CrossProduct3D(tangent, bitangent).GetNormalized();
	if (trueNormal.GetLengthSquared() == 0.f) {
		trueNormal = Vec3(0.f, 0.f, 1.f);
	}

	Vec3 midTop = (topLeft + topRight) * 0.5f;
	Vec3 midBottom = (bottomLeft + bottomRight) * 0.5f;
	float midU = (UVs.m_mins.x + UVs.m_maxs.x) * 0.5f;

	Vec3 leftEdgeNormal = -tangent;
	Vec3 rightEdgeNormal = tangent;

	Vec2 uvBL(UVs.m_mins.x, UVs.m_mins.y);
	Vec2 uvBM(midU, UVs.m_mins.y);
	Vec2 uvBR(UVs.m_maxs.x, UVs.m_mins.y);
	Vec2 uvTL(UVs.m_mins.x, UVs.m_maxs.y);
	Vec2 uvTM(midU, UVs.m_maxs.y);
	Vec2 uvTR(UVs.m_maxs.x, UVs.m_maxs.y);

	vertexes.push_back(Vertex(bottomLeft, color, uvBL, tangent, bitangent, leftEdgeNormal));
	vertexes.push_back(Vertex(midBottom, color, uvBM, tangent, bitangent, trueNormal));
	vertexes.push_back(Vertex(midTop, color, uvTM, tangent, bitangent, trueNormal));
	vertexes.push_back(Vertex(bottomLeft, color, uvBL, tangent, bitangent, leftEdgeNormal));
	vertexes.push_back(Vertex(midTop, color, uvTM, tangent, bitangent, trueNormal));
	vertexes.push_back(Vertex(topLeft, color, uvTL, tangent, bitangent, leftEdgeNormal));

	vertexes.push_back(Vertex(midBottom, color, uvBM, tangent, bitangent, trueNormal));
	vertexes.push_back(Vertex(bottomRight, color, uvBR, tangent, bitangent, rightEdgeNormal));
	vertexes.push_back(Vertex(topRight, color, uvTR, tangent, bitangent, rightEdgeNormal));
	vertexes.push_back(Vertex(midBottom, color, uvBM, tangent, bitangent, trueNormal));
	vertexes.push_back(Vertex(topRight, color, uvTR, tangent, bitangent, rightEdgeNormal));
	vertexes.push_back(Vertex(midTop, color, uvTM, tangent, bitangent, trueNormal));
}


void AddVertsForAABB3D(
	std::vector<Vertex>& verts,
	AABB3 const& bounds,
	Rgba8 const& color,
	AABB2 const& UVs)
{
	float minX = bounds.m_mins.x;
	float minY = bounds.m_mins.y;
	float minZ = bounds.m_mins.z;
	float maxX = bounds.m_maxs.x;
	float maxY = bounds.m_maxs.y;
	float maxZ = bounds.m_maxs.z;

	// +X
	AddVertsForQuad3D(verts,
		Vec3(maxX, minY, minZ),
		Vec3(maxX, maxY, minZ),
		Vec3(maxX, maxY, maxZ),
		Vec3(maxX, minY, maxZ),
		color,
		UVs);
	// -X
	AddVertsForQuad3D(verts,
		Vec3(minX, minY, maxZ),
		Vec3(minX, maxY, maxZ),
		Vec3(minX, maxY, minZ),
		Vec3(minX, minY, minZ),
		color,
		UVs);
	// +Y
	AddVertsForQuad3D(verts,
		Vec3(minX, maxY, minZ),
		Vec3(minX, maxY, maxZ),
		Vec3(maxX, maxY, maxZ),
		Vec3(maxX, maxY, minZ),
		color,
		UVs);
	// -Y
	AddVertsForQuad3D(verts,
		Vec3(minX, minY, maxZ),
		Vec3(minX, minY, minZ),
		Vec3(maxX, minY, minZ),
		Vec3(maxX, minY, maxZ),
		color,
		UVs);
	// +Z
	AddVertsForQuad3D(verts,
		Vec3(minX, minY, maxZ),
		Vec3(maxX, minY, maxZ),
		Vec3(maxX, maxY, maxZ),
		Vec3(minX, maxY, maxZ),
		color,
		UVs);
	// -Z
	AddVertsForQuad3D(verts,
		Vec3(maxX, minY, minZ),
		Vec3(minX, minY, minZ),
		Vec3(minX, maxY, minZ),
		Vec3(maxX, maxY, minZ),
		color,
		UVs);
}


void AddVertsForSphere3D(
	std::vector<Vertex>& verts,
	Vec3 const& center,
	float radius,
	Rgba8 const& color,
	AABB2 const& UVs,
	int numSlices,
	int numStacks)
{
	if (radius <= 0.f || numSlices < 3 || numStacks < 2) {
		return;
	}

	for (int stackIndex = 0; stackIndex < numStacks; ++stackIndex) {
		float v0 = static_cast<float>(stackIndex) / static_cast<float>(numStacks);
		float v1 = static_cast<float>(stackIndex + 1) / static_cast<float>(numStacks);
		float pitch0 = RangeMap(v0, 0.f, 1.f, -90.f, 90.f);
		float pitch1 = RangeMap(v1, 0.f, 1.f, -90.f, 90.f);

		for (int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex) {
			float u0 = static_cast<float>(sliceIndex) / static_cast<float>(numSlices);
			float u1 = static_cast<float>(sliceIndex + 1) / static_cast<float>(numSlices);
			float yaw0 = RangeMap(u0, 0.f, 1.f, 0.f, 360.f);
			float yaw1 = RangeMap(u1, 0.f, 1.f, 0.f, 360.f);

			Vec3 bottomLeft = center + Vec3::MakeFromPolarDegrees(pitch0, yaw0, radius);
			Vec3 bottomRight = center + Vec3::MakeFromPolarDegrees(pitch0, yaw1, radius);
			Vec3 topRight = center + Vec3::MakeFromPolarDegrees(pitch1, yaw1, radius);
			Vec3 topLeft = center + Vec3::MakeFromPolarDegrees(pitch1, yaw0, radius);

			// Keep engine-wide UV convention; flip sphere V so +Z (north pole) maps to texture top.
			float uvU0 = Interpolate(UVs.m_mins.x, UVs.m_maxs.x, u0);
			float uvU1 = Interpolate(UVs.m_mins.x, UVs.m_maxs.x, u1);
			float uvV0 = Interpolate(UVs.m_maxs.y, UVs.m_mins.y, v0);
			float uvV1 = Interpolate(UVs.m_maxs.y, UVs.m_mins.y, v1);
			AABB2 quadUVs(
				uvU0,
				uvV0,
				uvU1,
				uvV1);
			Vec2 uvBottomLeft(quadUVs.m_mins.x, quadUVs.m_mins.y);
			Vec2 uvBottomRight(quadUVs.m_maxs.x, quadUVs.m_mins.y);
			Vec2 uvTopRight(quadUVs.m_maxs.x, quadUVs.m_maxs.y);
			Vec2 uvTopLeft(quadUVs.m_mins.x, quadUVs.m_maxs.y);

			verts.push_back(Vertex(bottomLeft, color, uvBottomLeft));
			verts.push_back(Vertex(topRight, color, uvTopRight));
			verts.push_back(Vertex(bottomRight, color, uvBottomRight));

			verts.push_back(Vertex(bottomLeft, color, uvBottomLeft));
			verts.push_back(Vertex(topLeft, color, uvTopLeft));
			verts.push_back(Vertex(topRight, color, uvTopRight));
		}
	}
}

void TransformVertexArray3D(std::vector<Vertex>& verts, Mat44 const& transform)
{
	for (Vertex& vert : verts) {
		vert.m_position = transform.TransformPosition3D(vert.m_position);
	}
}

AABB2 GetVertexBounds2D(std::vector<Vertex> const& verts)
{
	if (verts.empty()) {
		return AABB2(Vec2::ZERO, Vec2::ZERO);
	}

	Vec2 mins(verts[0].m_position.x, verts[0].m_position.y);
	Vec2 maxs(verts[0].m_position.x, verts[0].m_position.y);
	for (Vertex const& vert : verts) {
		if (vert.m_position.x < mins.x) {
			mins.x = vert.m_position.x;
		}
		if (vert.m_position.y < mins.y) {
			mins.y = vert.m_position.y;
		}
		if (vert.m_position.x > maxs.x) {
			maxs.x = vert.m_position.x;
		}
		if (vert.m_position.y > maxs.y) {
			maxs.y = vert.m_position.y;
		}
	}

	return AABB2(mins, maxs);
}

void AddVertsForCylinder3D(std::vector<Vertex>& verts,
	Vec3 const& start, Vec3 const& end, float radius,
	Rgba8 const& color, AABB2 const& UVs, int numSlices)
{
	if (radius <= 0.f) {
		return;
	}

	Vec3 axis = end - start;
	float axisLength = axis.GetLength();
	if (axisLength <= 0.f) {
		return;
	}

	int slices = GetValidNumSlices(numSlices);
	Vec3 axisNormal = axis / axisLength;
	Vec3 iBasis;
	Vec3 jBasis;
	GetSurfaceTangentsForAxis(axisNormal, iBasis, jBasis);

	Vec2 centerUV = UVs.GetCenter();
	for (int sliceIndex = 0; sliceIndex < slices; ++sliceIndex) {
		float fraction0 = static_cast<float>(sliceIndex) / static_cast<float>(slices);
		float fraction1 = static_cast<float>(sliceIndex + 1) / static_cast<float>(slices);
		float theta0 = fraction0 * 360.f;
		float theta1 = fraction1 * 360.f;

		float cosTheta0 = CosDegrees(theta0);
		float sinTheta0 = SinDegrees(theta0);
		float cosTheta1 = CosDegrees(theta1);
		float sinTheta1 = SinDegrees(theta1);

		Vec3 radial0 = (cosTheta0 * iBasis) + (sinTheta0 * jBasis);
		Vec3 radial1 = (cosTheta1 * iBasis) + (sinTheta1 * jBasis);

		Vec3 start0 = start + (radius * radial0);
		Vec3 start1 = start + (radius * radial1);
		Vec3 end0 = end + (radius * radial0);
		Vec3 end1 = end + (radius * radial1);

		float uvU0 = Interpolate(UVs.m_mins.x, UVs.m_maxs.x, fraction0);
		float uvU1 = Interpolate(UVs.m_mins.x, UVs.m_maxs.x, fraction1);
		AABB2 sideUVs(uvU0, UVs.m_mins.y, uvU1, UVs.m_maxs.y);
		AddVertsForQuad3D(verts, start0, start1, end1, end0, color, sideUVs);

		Vec2 startUV0 = GetRadialUV(UVs, cosTheta0, sinTheta0);
		Vec2 startUV1 = GetRadialUV(UVs, cosTheta1, sinTheta1);

		verts.push_back(Vertex(start, color, centerUV));
		verts.push_back(Vertex(start1, color, startUV1));
		verts.push_back(Vertex(start0, color, startUV0));

		verts.push_back(Vertex(end, color, centerUV));
		verts.push_back(Vertex(end0, color, startUV0));
		verts.push_back(Vertex(end1, color, startUV1));
	}
}

void AddVertsForCone3D(std::vector<Vertex>& verts,
	Vec3 const& start, Vec3 const& end, float radius,
	Rgba8 const& color, AABB2 const& UVs, int numSlices)
{
	if (radius <= 0.f) {
		return;
	}

	Vec3 axis = end - start;
	float axisLength = axis.GetLength();
	if (axisLength <= 0.f) {
		return;
	}

	int slices = GetValidNumSlices(numSlices);
	Vec3 axisNormal = axis / axisLength;
	Vec3 iBasis;
	Vec3 jBasis;
	GetSurfaceTangentsForAxis(axisNormal, iBasis, jBasis);

	Vec2 centerUV = UVs.GetCenter();
	for (int sliceIndex = 0; sliceIndex < slices; ++sliceIndex) {
		float fraction0 = static_cast<float>(sliceIndex) / static_cast<float>(slices);
		float fraction1 = static_cast<float>(sliceIndex + 1) / static_cast<float>(slices);
		float theta0 = fraction0 * 360.f;
		float theta1 = fraction1 * 360.f;

		float cosTheta0 = CosDegrees(theta0);
		float sinTheta0 = SinDegrees(theta0);
		float cosTheta1 = CosDegrees(theta1);
		float sinTheta1 = SinDegrees(theta1);

		Vec3 baseRadial0 = (cosTheta0 * iBasis) + (sinTheta0 * jBasis);
		Vec3 baseRadial1 = (cosTheta1 * iBasis) + (sinTheta1 * jBasis);
		Vec3 base0 = start + (radius * baseRadial0);
		Vec3 base1 = start + (radius * baseRadial1);

		float uvU0 = Interpolate(UVs.m_mins.x, UVs.m_maxs.x, fraction0);
		float uvU1 = Interpolate(UVs.m_mins.x, UVs.m_maxs.x, fraction1);
		float tipU = 0.5f * (uvU0 + uvU1);
		verts.push_back(Vertex(base0, color, Vec2(uvU0, UVs.m_mins.y)));
		verts.push_back(Vertex(base1, color, Vec2(uvU1, UVs.m_mins.y)));
		verts.push_back(Vertex(end, color, Vec2(tipU, UVs.m_maxs.y)));

		Vec2 baseUV0 = GetRadialUV(UVs, cosTheta0, sinTheta0);
		Vec2 baseUV1 = GetRadialUV(UVs, cosTheta1, sinTheta1);
		verts.push_back(Vertex(start, color, centerUV));
		verts.push_back(Vertex(base1, color, baseUV1));
		verts.push_back(Vertex(base0, color, baseUV0));
	}
}

void AddVertsForArrow3D(std::vector<Vertex>& verts,
	Vec3 const& start, Vec3 const& end, float radius,
	Rgba8 const& color, int numSlices)
{
	if (radius <= 0.f) {
		return;
	}

	Vec3 axis = end - start;
	float axisLength = axis.GetLength();
	if (axisLength <= 0.f) {
		return;
	}

	int slices = GetValidNumSlices(numSlices);
	float desiredHeadLength = radius * 4.f;
	float minHeadLength = radius * 1.5f;
	float maxHeadLength = axisLength - (radius * 0.25f);
	if (maxHeadLength <= 0.f || axisLength <= minHeadLength) {
		AddVertsForCone3D(verts, start, end, radius * 2.f, color, AABB2(0.f, 0.f, 1.f, 1.f), slices);
		return;
	}

	float headLength = desiredHeadLength;
	if (maxHeadLength < minHeadLength) {
		headLength = maxHeadLength;
	}
	else {
		headLength = GetClamped(desiredHeadLength, minHeadLength, maxHeadLength);
	}

	Vec3 axisNormal = axis.GetNormalized();
	Vec3 coneStart = end - (axisNormal * headLength);
	AddVertsForCylinder3D(verts, start, coneStart, radius, color, AABB2(0.f, 0.f, 1.f, 1.f), slices);
	AddVertsForCone3D(verts, coneStart, end, radius * 2.f, color, AABB2(0.f, 0.f, 1.f, 1.f), slices);
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

void AddVertsForArrow2D(std::vector<Vertex>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 color)
{
	AddVertsForLineSegment2D(verts, tailPos, tipPos, lineThickness, color);

	Vec2 fwdNormal	= (tipPos - tailPos).GetNormalized();
	Vec2 a1 = tipPos + (fwdNormal).GetRotatedByDegrees(135.f) * arrowSize;
	Vec2 a2 = tipPos + (fwdNormal).GetRotatedByDegrees(225.f) * arrowSize;

	AddVertsForLineSegment2D(verts, a1, tipPos, lineThickness, color);
	AddVertsForLineSegment2D(verts, a2, tipPos, lineThickness, color);
}

//-----------------------------------------------------------------------------------------------
void AddVertsForOBB3(std::vector<Vertex>& verts, OBB3 const& box, Rgba8 const& color, AABB2 const& UVs)
{
	Vec3 corners[8];
	box.GetCornerPositions(corners);

	AddVertsForQuad3D(verts, corners[1], corners[2], corners[6], corners[5], color, UVs); // +i face
	AddVertsForQuad3D(verts, corners[3], corners[0], corners[4], corners[7], color, UVs); // -i face
	AddVertsForQuad3D(verts, corners[2], corners[3], corners[7], corners[6], color, UVs); // +j face
	AddVertsForQuad3D(verts, corners[0], corners[1], corners[5], corners[4], color, UVs); // -j face
	AddVertsForQuad3D(verts, corners[4], corners[5], corners[6], corners[7], color, UVs); // +k face
	AddVertsForQuad3D(verts, corners[3], corners[2], corners[1], corners[0], color, UVs); // -k face
}

void AddVertsForPlane3(std::vector<Vertex>& verts, Plane3 const& plane,
	float gridHalfExtent, float gridStepLength, float lineThickness, Rgba8 const& color)
{
	Vec3 centerOnPlane = plane.m_normal * plane.m_distanceFromOrigin;

	Vec3 reference = (fabsf(plane.m_normal.z) < 0.9f) ? Vec3(0.f, 0.f, 1.f) : Vec3(1.f, 0.f, 0.f);
	Vec3 uAxis = CrossProduct3D(plane.m_normal, reference);
	if (uAxis.GetLengthSquared() == 0.f) {
		reference = Vec3(0.f, 1.f, 0.f);
		uAxis = CrossProduct3D(plane.m_normal, reference);
	}
	uAxis = uAxis.GetNormalized();
	Vec3 vAxis = CrossProduct3D(plane.m_normal, uAxis).GetNormalized();

	int numLinesPerSide = static_cast<int>(gridHalfExtent / gridStepLength);
	for (int i = -numLinesPerSide; i <= numLinesPerSide; ++i) {
		float offset = static_cast<float>(i) * gridStepLength;
		Vec3 alongU0 = centerOnPlane + (vAxis * offset) - (uAxis * gridHalfExtent);
		Vec3 alongU1 = centerOnPlane + (vAxis * offset) + (uAxis * gridHalfExtent);
		AddVertsForCylinder3D(verts, alongU0, alongU1, lineThickness, color, AABB2(0.f, 0.f, 1.f, 1.f), 6);

		Vec3 alongV0 = centerOnPlane + (uAxis * offset) - (vAxis * gridHalfExtent);
		Vec3 alongV1 = centerOnPlane + (uAxis * offset) + (vAxis * gridHalfExtent);
		AddVertsForCylinder3D(verts, alongV0, alongV1, lineThickness, color, AABB2(0.f, 0.f, 1.f, 1.f), 6);
	}
}

