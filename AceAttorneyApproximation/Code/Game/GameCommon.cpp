#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include <Engine/Math/MathUtils.hpp>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/UI/UITheme.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Prop.hpp"

//-----------------------------------------------------------------------------------------------
UITheme* g_uiTheme = nullptr;

//-----------------------------------------------------------------------------------------------
static int GetAbsInt(int value)
{
	if (value < 0) {
		return -value;
	}

	return value;
}

//-----------------------------------------------------------------------------------------------
static void GetGridLineStyle(
	int lineIndex,
	GridPropConfig const& config,
	float& outThickness,
	unsigned char& outIntensity,
	bool& outIsGrayLine)
{
	int majorInterval = config.m_majorLineInterval;
	if (majorInterval <= 0) {
		majorInterval = 1;
	}

	float minorThickness = config.m_minorLineThickness;
	if (minorThickness <= 0.0f) {
		minorThickness = 0.01f;
	}

	outThickness = minorThickness;
	outIntensity = config.m_minorGrayIntensity;
	outIsGrayLine = true;

	if (lineIndex == 0) {
		float axisThickness = config.m_axisLineThickness;
		if (axisThickness > 0.0f) {
			outThickness = axisThickness;
		}
		outIntensity = config.m_axisIntensity;
		outIsGrayLine = false;
		return;
	}

	if ((GetAbsInt(lineIndex) % majorInterval) == 0) {
		float majorThickness = config.m_majorLineThickness;
		if (majorThickness > 0.0f) {
			outThickness = majorThickness;
		}
		outIntensity = config.m_majorRedGreenIntensity;
		outIsGrayLine = false;
	}
}

//-----------------------------------------------------------------------------------------------
Prop* CreateCubeProp(
	Game* owner,
	float sideLength,
	Texture* texture,
	AABB2 const& uvs,
	Rgba8 const& positiveXColor,
	Rgba8 const& negativeXColor,
	Rgba8 const& positiveYColor,
	Rgba8 const& negativeYColor,
	Rgba8 const& positiveZColor,
	Rgba8 const& negativeZColor)
{
	Prop* cube = new Prop(owner);
	cube->m_texture = texture;
	if (sideLength <= 0.0f) {
		sideLength = 1.0f;
	}

	float mins = -0.5f * sideLength;
	float maxs = 0.5f * sideLength;

	// +X face
	AddVertsForQuad3D(
		cube->m_vertexes,
		Vec3(maxs, mins, mins),
		Vec3(maxs, maxs, mins),
		Vec3(maxs, maxs, maxs),
		Vec3(maxs, mins, maxs),
		positiveXColor,
		uvs);
	// -X face
	AddVertsForQuad3D(
		cube->m_vertexes,
		Vec3(mins, mins, maxs),
		Vec3(mins, maxs, maxs),
		Vec3(mins, maxs, mins),
		Vec3(mins, mins, mins),
		negativeXColor,
		uvs);
	// +Y face
	AddVertsForQuad3D(
		cube->m_vertexes,
		Vec3(mins, maxs, mins),
		Vec3(mins, maxs, maxs),
		Vec3(maxs, maxs, maxs),
		Vec3(maxs, maxs, mins),
		positiveYColor,
		uvs);
	// -Y face
	AddVertsForQuad3D(
		cube->m_vertexes,
		Vec3(mins, mins, maxs),
		Vec3(mins, mins, mins),
		Vec3(maxs, mins, mins),
		Vec3(maxs, mins, maxs),
		negativeYColor,
		uvs);
	// +Z face
	AddVertsForQuad3D(
		cube->m_vertexes,
		Vec3(mins, mins, maxs),
		Vec3(maxs, mins, maxs),
		Vec3(maxs, maxs, maxs),
		Vec3(mins, maxs, maxs),
		positiveZColor,
		uvs);
	// -Z face
	AddVertsForQuad3D(
		cube->m_vertexes,
		Vec3(maxs, mins, mins),
		Vec3(mins, mins, mins),
		Vec3(mins, maxs, mins),
		Vec3(maxs, maxs, mins),
		negativeZColor,
		uvs);

	return cube;
}

//-----------------------------------------------------------------------------------------------
Prop* CreateSphereProp(
	Game* owner,
	float radius,
	int numSlices,
	int numStacks,
	Texture* texture,
	AABB2 const& uvs,
	Rgba8 const& color)
{
	Prop* sphere = new Prop(owner);
	sphere->m_texture = texture;
	if (radius <= 0.0f) {
		radius = 1.0f;
	}
	if (numSlices < 3) {
		numSlices = 3;
	}
	if (numStacks < 2) {
		numStacks = 2;
	}

	AddVertsForSphere3D(
		sphere->m_vertexes,
		Vec3(0.0f, 0.0f, 0.0f),
		radius,
		color,
		uvs,
		numSlices,
		numStacks);

	return sphere;
}

//-----------------------------------------------------------------------------------------------
Prop* CreateGridProp(Game* owner, GridPropConfig const& config)
{
	Prop* grid = new Prop(owner);

	float spacing = config.m_spacing;
	if (spacing <= 0.0f) {
		spacing = 1.0f;
	}

	float halfExtentInput = config.m_halfExtent;
	if (halfExtentInput <= 0.0f) {
		halfExtentInput = 1.0f;
	}

	int lineCountPerSide = static_cast<int>(halfExtentInput / spacing);
	if (lineCountPerSide < 1) {
		lineCountPerSide = 1;
	}

	float halfExtent = static_cast<float>(lineCountPerSide) * spacing;
	int totalLineCount = (lineCountPerSide * 2) + 1;
	int estimatedVertexCount = totalLineCount * 2 * 36;
	grid->m_vertexes.reserve(static_cast<size_t>(estimatedVertexCount));

	for (int lineIndex = -lineCountPerSide; lineIndex <= lineCountPerSide; ++lineIndex) {
		float thickness = config.m_minorLineThickness;
		unsigned char intensity = config.m_minorGrayIntensity;
		bool isGrayLine = true;
		GetGridLineStyle(lineIndex, config, thickness, intensity, isGrayLine);

		float halfThickness = 0.5f * thickness;
		float lineCoord = static_cast<float>(lineIndex) * spacing;
		Rgba8 xLineColor;
		Rgba8 yLineColor;
		if (isGrayLine) {
			xLineColor = Rgba8(intensity, intensity, intensity, 255);
			yLineColor = xLineColor;
		}
		else {
			xLineColor = Rgba8(intensity, 0, 0, 255);
			yLineColor = Rgba8(0, intensity, 0, 255);
		}

		AddVertsForAABB3D(
			grid->m_vertexes,
			AABB3(
				Vec3(-halfExtent, lineCoord - halfThickness, -halfThickness),
				Vec3(halfExtent, lineCoord + halfThickness, halfThickness)),
			xLineColor);
		AddVertsForAABB3D(
			grid->m_vertexes,
			AABB3(
				Vec3(lineCoord - halfThickness, -halfExtent, -halfThickness),
				Vec3(lineCoord + halfThickness, halfExtent, halfThickness)),
			yLineColor);
	}

	return grid;
}

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
