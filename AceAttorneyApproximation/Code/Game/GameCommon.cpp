#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include <Engine/Math/MathUtils.hpp>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"

//-----------------------------------------------------------------------------------------------
void DebugDrawLine(Vec2 const& startPos, Vec2 const& endPos, float const& thickness, Rgba8 const& startColor, Rgba8 const& endColor)
{
	// Compute (half-thickness-long) forward and left displacement
	Vec2 fowardNormal = (endPos - startPos).GetNormalized();
	Vec2 fowardStep = fowardNormal * (thickness * 0.5f);
	Vec2 leftStep = fowardStep.GetRotatedBy90Degrees();

	// Compute four corner positions
	Vec2 endLeft = endPos + fowardStep + leftStep;
	Vec2 endRight = endPos + fowardStep - leftStep;
	Vec2 startLeft = startPos - fowardStep + leftStep;
	Vec2 startRight = startPos - fowardStep - leftStep;

	// Create vertexes, then draw
	Vertex verts[6];

	verts[0].m_position = Vec3(startRight);
	verts[1].m_position = Vec3(endRight); 
	verts[2].m_position = Vec3(endLeft);
	verts[3].m_position = Vec3(startRight);
	verts[4].m_position = Vec3(endLeft); 
	verts[5].m_position = Vec3(startLeft);

	verts[0].m_color = startColor; 
	verts[1].m_color = endColor; 
	verts[2].m_color = endColor; 
	verts[3].m_color = startColor; 
	verts[4].m_color = endColor; 
	verts[5].m_color = startColor;

	verts[0].m_uvTexCoords = Vec2(0.f, 0.f);
	verts[1].m_uvTexCoords = Vec2(1.f, 0.f);
	verts[2].m_uvTexCoords = Vec2(1.f, 1.f);
	verts[3].m_uvTexCoords = Vec2(0.f, 0.f);
	verts[4].m_uvTexCoords = Vec2(1.f, 1.f);
	verts[5].m_uvTexCoords = Vec2(0.f, 1.f);

	g_engine->m_render->DrawVertexArray(6, verts);
}

void DebugDrawRing(Vec2 const& originPos, float const& radius, float const& thickness, Rgba8 const& color)
{
	DrawFadedRing(originPos, radius - 0.5f * thickness, radius + 0.5f * thickness,color, color);
}

void DrawFadedRing(Vec2 const& originPos, float const& innerRadius, float const& outerRadius, Rgba8 const& innerColor, Rgba8 const& outerColor)
{
	float deltaTheta = 360.f / 64;
	// float radius = 0.5 * (innerRadius + outerRadius);
	for (int i = 0; i < 64; ++i) {
		
		Vec2 innerStartPos = originPos + innerRadius * Vec2(CosDegrees(i * deltaTheta), SinDegrees(i * deltaTheta));
		Vec2 innerEndPos = originPos + innerRadius * Vec2(CosDegrees((i + 1) * deltaTheta), SinDegrees((i + 1) * deltaTheta));
		Vec2 outerStartPos = originPos + outerRadius * Vec2(CosDegrees(i * deltaTheta), SinDegrees(i * deltaTheta));
		Vec2 outerEndPos = originPos + outerRadius * Vec2(CosDegrees((i + 1) * deltaTheta), SinDegrees((i + 1) * deltaTheta));

		// Create vertexes, then draw
		Vertex verts[6];

		verts[0].m_position = Vec3(innerStartPos);
		verts[1].m_position = Vec3(innerEndPos);
		verts[2].m_position = Vec3(outerStartPos);
		verts[3].m_position = Vec3(innerEndPos);
		verts[4].m_position = Vec3(outerStartPos);
		verts[5].m_position = Vec3(outerEndPos);

		verts[0].m_color = innerColor;
		verts[1].m_color = innerColor;
		verts[2].m_color = outerColor;
		verts[3].m_color = innerColor;
		verts[4].m_color = outerColor;
		verts[5].m_color = outerColor;

		verts[0].m_uvTexCoords = Vec2(0.f, 0.f);
		verts[1].m_uvTexCoords = Vec2(1.f, 0.f);
		verts[2].m_uvTexCoords = Vec2(1.f, 1.f);
		verts[3].m_uvTexCoords = Vec2(0.f, 0.f);
		verts[4].m_uvTexCoords = Vec2(1.f, 1.f);
		verts[5].m_uvTexCoords = Vec2(0.f, 1.f);

		g_engine->m_render->DrawVertexArray(6, verts);
	}
}

void DrawGlow(Vec2 pos, Rgba8 color, float alpha, float radius)
{
	color.ScaleAlpha(alpha);

	float innerRadius = 0.f,
		outerRadius = radius;
	Rgba8 innerColor = color,
		outerColor(innerColor.r, innerColor.g, innerColor.b, 0);

	DrawFadedRing(pos, innerRadius, outerRadius, innerColor, outerColor);
}
