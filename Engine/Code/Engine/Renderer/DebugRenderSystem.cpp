#include "Engine/Renderer/DebugRenderSystem.hpp"

#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace
{
enum class DebugObjectType
{
	WORLD_GEOMETRY,
	WORLD_TEXT,
	WORLD_BILLBOARD_TEXT,
	SCREEN_TEXT,
	MESSAGE,
};

struct DebugObject
{
	DebugObjectType m_type = DebugObjectType::WORLD_GEOMETRY;
	DebugRenderMode m_mode = DebugRenderMode::USE_DEPTH;

	float m_durationSeconds = 0.f;
	float m_elapsedSeconds = 0.f;
	bool m_isInfinite = false;
	bool m_isOneFrame = false;
	uint64_t m_addOrder = 0u;

	Rgba8 m_startColor = Rgba8::WHITE;
	Rgba8 m_endColor = Rgba8::WHITE;

	std::vector<Vertex> m_geometryVerts;
	std::string m_text;
	Mat44 m_transform;
	Vec3 m_billboardOrigin = Vec3(0.f, 0.f, 0.f);
	AABB2 m_screenBox;
	float m_textHeight = 0.f;
	float m_screenCellHeight = 0.f;
	Vec2 m_alignment = Vec2(0.5f, 0.5f);
};

struct DebugRenderSystemState
{
	DebugRenderConfig m_config;
	bool m_isVisible = true;
	uint64_t m_nextAddOrder = 1u;
	BitmapFont* m_font = nullptr;
	std::string m_fontPathWithName;
	std::vector<DebugObject> m_objects;
};

constexpr float DEBUG_WIRE_SEGMENT_RADIUS_SCALE = 0.012f;
constexpr float DEBUG_WIRE_SEGMENT_RADIUS_MIN = 0.0018f;
constexpr int DEBUG_CIRCLE_SEGMENTS = 40;
constexpr int DEBUG_CIRCLE_CYLINDER_SIDES = 6;
constexpr int DEBUG_WIRE_SPHERE_SLICES = 18;
constexpr int DEBUG_WIRE_SPHERE_STACKS = 10;
constexpr int DEBUG_WIRE_CYLINDER_SIDE_LINES = 20;
constexpr int DEBUG_WIRE_CYLINDER_CAP_RADIAL_LINES = 16;
constexpr float DEBUG_MESSAGE_MARGIN = 8.f;
constexpr float DEBUG_MESSAGE_LINE_HEIGHT = 18.f;
constexpr float DEBUG_MESSAGE_LINE_SPACING = 2.f;

constexpr char const* DEBUG_COMMAND_CLEAR = "DebugRenderClear";
constexpr char const* DEBUG_COMMAND_TOGGLE = "DebugRenderToggle";
constexpr char const* DEBUG_COMMAND_CLEAR_ALIAS = "Clear";
constexpr char const* DEBUG_COMMAND_TOGGLE_ALIAS = "Toggle";

DebugRenderSystemState* g_debugRenderState = nullptr;

float GetWireSegmentRadius(float radius)
{
	float wireRadius = radius * DEBUG_WIRE_SEGMENT_RADIUS_SCALE;
	if (wireRadius < DEBUG_WIRE_SEGMENT_RADIUS_MIN) {
		wireRadius = DEBUG_WIRE_SEGMENT_RADIUS_MIN;
	}

	return wireRadius;
}

Vec3 GetNormalizedOrFallback(Vec3 const& value, Vec3 const& fallback)
{
	Vec3 normalized = value.GetNormalized();
	if (normalized.GetLengthSquared() == 0.f) {
		return fallback;
	}

	return normalized;
}

unsigned char ScaleColorByte(unsigned char channel, float scale)
{
	float scaledValue = static_cast<float>(channel) * scale;
	scaledValue = GetClamped(scaledValue, 0.f, 255.f);
	return static_cast<unsigned char>(scaledValue);
}

Rgba8 ScaleColor(Rgba8 const& color, float rgbScale, float alphaScale)
{
	return Rgba8(
		ScaleColorByte(color.r, rgbScale),
		ScaleColorByte(color.g, rgbScale),
		ScaleColorByte(color.b, rgbScale),
		ScaleColorByte(color.a, alphaScale));
}

void GetPerpendicularBasis(Vec3 const& axisNormal, Vec3& out_jBasis, Vec3& out_kBasis)
{
	Vec3 reference = fabsf(axisNormal.z) < 0.999f ? Vec3(0.f, 0.f, 1.f) : Vec3(0.f, 1.f, 0.f);
	out_jBasis = GetNormalizedOrFallback(CrossProduct3D(reference, axisNormal), Vec3(0.f, 1.f, 0.f));
	out_kBasis = GetNormalizedOrFallback(CrossProduct3D(axisNormal, out_jBasis), Vec3(0.f, 0.f, 1.f));
}

void AddWireCircle3D(
	std::vector<Vertex>& verts,
	Vec3 const& center,
	Vec3 const& jBasisNormal,
	Vec3 const& kBasisNormal,
	float radius,
	float wireRadius,
	Rgba8 const& color)
{
	if (radius <= 0.f) {
		return;
	}

	for (int segmentIndex = 0; segmentIndex < DEBUG_CIRCLE_SEGMENTS; ++segmentIndex) {
		float fractionA = static_cast<float>(segmentIndex) / static_cast<float>(DEBUG_CIRCLE_SEGMENTS);
		float fractionB = static_cast<float>(segmentIndex + 1) / static_cast<float>(DEBUG_CIRCLE_SEGMENTS);
		float angleDegreesA = 360.f * fractionA;
		float angleDegreesB = 360.f * fractionB;

		Vec3 pointA =
			center +
			(jBasisNormal * (radius * CosDegrees(angleDegreesA))) +
			(kBasisNormal * (radius * SinDegrees(angleDegreesA)));
		Vec3 pointB =
			center +
			(jBasisNormal * (radius * CosDegrees(angleDegreesB))) +
			(kBasisNormal * (radius * SinDegrees(angleDegreesB)));

		AddVertsForCylinder3D(verts, pointA, pointB, wireRadius, color, AABB2(0.f, 0.f, 1.f, 1.f), DEBUG_CIRCLE_CYLINDER_SIDES);
	}
}

void AddWireSphere3D(std::vector<Vertex>& verts, Vec3 const& center, float radius, Rgba8 const& color)
{
	if (radius <= 0.f) {
		return;
	}

	float wireRadius = GetWireSegmentRadius(radius);

	// Latitude rings (parallel circles)
	for (int stackIndex = 1; stackIndex < DEBUG_WIRE_SPHERE_STACKS; ++stackIndex) {
		float stackFraction = static_cast<float>(stackIndex) / static_cast<float>(DEBUG_WIRE_SPHERE_STACKS);
		float pitchDegrees = RangeMap(stackFraction, 0.f, 1.f, -90.f, 90.f);
		float ringRadius = radius * CosDegrees(pitchDegrees);
		float ringZ = radius * SinDegrees(pitchDegrees);
		if (ringRadius <= 0.f) {
			continue;
		}

		AddWireCircle3D(
			verts,
			center + Vec3(0.f, 0.f, ringZ),
			Vec3(1.f, 0.f, 0.f),
			Vec3(0.f, 1.f, 0.f),
			ringRadius,
			wireRadius,
			color);
	}

	// Longitude rings (meridians)
	for (int sliceIndex = 0; sliceIndex < DEBUG_WIRE_SPHERE_SLICES; ++sliceIndex) {
		float sliceFraction = static_cast<float>(sliceIndex) / static_cast<float>(DEBUG_WIRE_SPHERE_SLICES);
		float yawDegrees = 360.f * sliceFraction;
		Vec3 meridianDir = Vec3(CosDegrees(yawDegrees), SinDegrees(yawDegrees), 0.f);

		for (int stackSegment = 0; stackSegment < DEBUG_WIRE_SPHERE_STACKS; ++stackSegment) {
			float segmentStartFraction = static_cast<float>(stackSegment) / static_cast<float>(DEBUG_WIRE_SPHERE_STACKS);
			float segmentEndFraction = static_cast<float>(stackSegment + 1) / static_cast<float>(DEBUG_WIRE_SPHERE_STACKS);
			float pitchA = RangeMap(segmentStartFraction, 0.f, 1.f, -90.f, 90.f);
			float pitchB = RangeMap(segmentEndFraction, 0.f, 1.f, -90.f, 90.f);

			float cosPitchA = CosDegrees(pitchA);
			float cosPitchB = CosDegrees(pitchB);
			Vec3 pointA = center + Vec3(
				meridianDir.x * radius * cosPitchA,
				meridianDir.y * radius * cosPitchA,
				radius * SinDegrees(pitchA));
			Vec3 pointB = center + Vec3(
				meridianDir.x * radius * cosPitchB,
				meridianDir.y * radius * cosPitchB,
				radius * SinDegrees(pitchB));

			AddVertsForCylinder3D(
				verts,
				pointA,
				pointB,
				wireRadius,
				color,
				AABB2(0.f, 0.f, 1.f, 1.f),
				DEBUG_CIRCLE_CYLINDER_SIDES);
		}
	}
}

void AddWireCylinder3D(std::vector<Vertex>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color)
{
	Vec3 axis = end - start;
	float axisLength = axis.GetLength();
	if (axisLength <= 0.f || radius <= 0.f) {
		return;
	}

	Vec3 axisNormal = axis / axisLength;
	Vec3 jBasis;
	Vec3 kBasis;
	GetPerpendicularBasis(axisNormal, jBasis, kBasis);

	float wireRadius = GetWireSegmentRadius(radius);
	AddWireCircle3D(verts, start, jBasis, kBasis, radius, wireRadius, color);
	AddWireCircle3D(verts, end, jBasis, kBasis, radius, wireRadius, color);

	for (int sideIndex = 0; sideIndex < DEBUG_WIRE_CYLINDER_SIDE_LINES; ++sideIndex) {
		float sideFraction = static_cast<float>(sideIndex) / static_cast<float>(DEBUG_WIRE_CYLINDER_SIDE_LINES);
		float sideAngleDegrees = 360.f * sideFraction;
		Vec3 offset =
			(jBasis * (radius * CosDegrees(sideAngleDegrees))) +
			(kBasis * (radius * SinDegrees(sideAngleDegrees)));

		AddVertsForCylinder3D(
			verts,
			start + offset,
			end + offset,
			wireRadius,
			color,
			AABB2(0.f, 0.f, 1.f, 1.f),
			DEBUG_CIRCLE_CYLINDER_SIDES);
	}

	for (int radialIndex = 0; radialIndex < DEBUG_WIRE_CYLINDER_CAP_RADIAL_LINES; ++radialIndex) {
		float radialFraction = static_cast<float>(radialIndex) / static_cast<float>(DEBUG_WIRE_CYLINDER_CAP_RADIAL_LINES);
		float radialAngleDegrees = 360.f * radialFraction;
		Vec3 radialOffset =
			(jBasis * (radius * CosDegrees(radialAngleDegrees))) +
			(kBasis * (radius * SinDegrees(radialAngleDegrees)));

		AddVertsForCylinder3D(
			verts,
			start,
			start + radialOffset,
			wireRadius,
			color,
			AABB2(0.f, 0.f, 1.f, 1.f),
			DEBUG_CIRCLE_CYLINDER_SIDES);
		AddVertsForCylinder3D(
			verts,
			end,
			end + radialOffset,
			wireRadius,
			color,
			AABB2(0.f, 0.f, 1.f, 1.f),
			DEBUG_CIRCLE_CYLINDER_SIDES);
	}
}

void AddWireCone3D(std::vector<Vertex>& verts, Vec3 const& baseCenter, Vec3 const& tip, float baseRadius, Rgba8 const& color)
{
	Vec3 axis = tip - baseCenter;
	float axisLength = axis.GetLength();
	if (axisLength <= 0.f || baseRadius <= 0.f) {
		return;
	}

	Vec3 axisNormal = axis / axisLength;
	Vec3 jBasis;
	Vec3 kBasis;
	GetPerpendicularBasis(axisNormal, jBasis, kBasis);

	float wireRadius = GetWireSegmentRadius(baseRadius);
	AddWireCircle3D(verts, baseCenter, jBasis, kBasis, baseRadius, wireRadius, color);

	const int spokeCount = 8;
	for (int spokeIndex = 0; spokeIndex < spokeCount; ++spokeIndex) {
		float spokeFraction = static_cast<float>(spokeIndex) / static_cast<float>(spokeCount);
		float spokeAngleDegrees = 360.f * spokeFraction;
		Vec3 ringPoint =
			baseCenter +
			(jBasis * (baseRadius * CosDegrees(spokeAngleDegrees))) +
			(kBasis * (baseRadius * SinDegrees(spokeAngleDegrees)));
		AddVertsForCylinder3D(
			verts,
			ringPoint,
			tip,
			wireRadius,
			color,
			AABB2(0.f, 0.f, 1.f, 1.f),
			DEBUG_CIRCLE_CYLINDER_SIDES);
	}
}

void AddWireArrow3D(std::vector<Vertex>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color)
{
	Vec3 axis = end - start;
	float axisLength = axis.GetLength();
	if (axisLength <= 0.f || radius <= 0.f) {
		return;
	}

	Vec3 axisNormal = axis / axisLength;
	float headLength = radius * 2.5f;
	if (headLength < axisLength * 0.25f) {
		headLength = axisLength * 0.25f;
	}
	if (headLength > axisLength * 0.8f) {
		headLength = axisLength * 0.8f;
	}

	Vec3 shaftEnd = end - (axisNormal * headLength);
	AddWireCylinder3D(verts, start, shaftEnd, radius, color);
	AddWireCone3D(verts, shaftEnd, end, radius * 2.f, color);
}

bool IsSystemReady()
{
	return g_debugRenderState != nullptr && g_debugRenderState->m_config.m_renderer != nullptr;
}

BitmapFont* GetDebugFont()
{
	if (!IsSystemReady()) {
		return nullptr;
	}

	if (g_debugRenderState->m_font == nullptr) {
		Renderer* renderer = g_debugRenderState->m_config.m_renderer;
		g_debugRenderState->m_font = renderer->CreateOrGetBitmapFont(g_debugRenderState->m_fontPathWithName.c_str());
	}

	return g_debugRenderState->m_font;
}

float GetLifetimeFraction(DebugObject const& object)
{
	if (object.m_isInfinite || object.m_isOneFrame || object.m_durationSeconds <= 0.f) {
		return 0.f;
	}

	return GetClampedZeroToOne(object.m_elapsedSeconds / object.m_durationSeconds);
}

Rgba8 GetCurrentColor(DebugObject const& object)
{
	float colorFraction = GetLifetimeFraction(object);
	return Interpolate(object.m_startColor, object.m_endColor, colorFraction);
}

void RemoveExpiredFiniteObjects()
{
	if (!IsSystemReady()) {
		return;
	}

	float deltaSeconds = static_cast<float>(Clock::GetSystemClock().GetDeltaSeconds());
	if (deltaSeconds < 0.f) {
		deltaSeconds = 0.f;
	}

	std::vector<DebugObject>& objects = g_debugRenderState->m_objects;
	auto eraseBegin = std::remove_if(
		objects.begin(),
		objects.end(),
		[deltaSeconds](DebugObject& object)
		{
			if (object.m_isInfinite || object.m_isOneFrame) {
				return false;
			}

			object.m_elapsedSeconds += deltaSeconds;
			return object.m_elapsedSeconds >= object.m_durationSeconds;
		});
	objects.erase(eraseBegin, objects.end());
}

void RemoveOneFrameObjects()
{
	if (!IsSystemReady()) {
		return;
	}

	std::vector<DebugObject>& objects = g_debugRenderState->m_objects;
	auto eraseBegin = std::remove_if(
		objects.begin(),
		objects.end(),
		[](DebugObject const& object)
		{
			return object.m_isOneFrame;
		});
	objects.erase(eraseBegin, objects.end());
}

DebugObject CreateDebugObject(
	DebugObjectType type,
	float duration,
	Rgba8 const& startColor,
	Rgba8 const& endColor,
	DebugRenderMode mode)
{
	DebugObject object;
	object.m_type = type;
	object.m_mode = mode;
	object.m_durationSeconds = duration;
	object.m_startColor = startColor;
	object.m_endColor = endColor;
	object.m_isInfinite = duration < 0.f;
	object.m_isOneFrame = duration == 0.f;
	return object;
}

void AddObject(DebugObject&& object)
{
	if (!IsSystemReady()) {
		return;
	}

	object.m_addOrder = g_debugRenderState->m_nextAddOrder++;
	g_debugRenderState->m_objects.push_back(std::move(object));
}

void ApplyColorToVerts(std::vector<Vertex>& verts, Rgba8 const& color)
{
	int const vertCount = static_cast<int>(verts.size());
	for (int vertIndex = 0; vertIndex < vertCount; ++vertIndex) {
		verts[vertIndex].m_color = color;
	}
}

void DrawWorldVertsByMode(
	Renderer& renderer,
	std::vector<Vertex> const& verts,
	Mat44 const& modelTransform,
	DebugRenderMode mode,
	Texture const* texture)
{
	if (verts.empty()) {
		return;
	}

	renderer.SetModelConstants(modelTransform, Rgba8::WHITE);
	renderer.BindTexture(texture);

	switch (mode)
	{
	case DebugRenderMode::ALWAYS:
		renderer.SetBlendMode(BlendMode::ALPHA);
		renderer.SetDepthMode(DepthMode::DISABLED);
		renderer.DrawVertexArray(verts);
		break;

	case DebugRenderMode::XRAY:
	{
		std::vector<Vertex> xrayVerts = verts;
		for (Vertex& vert : xrayVerts) {
			Rgba8 xrayColor = Interpolate(vert.m_color, Rgba8::WHITE, 0.5f);
			xrayColor.ScaleAlpha(0.5f);
			vert.m_color = xrayColor;
		}

		renderer.SetBlendMode(BlendMode::ALPHA);
		renderer.SetDepthMode(DepthMode::READ_ONLY_LESS_EQUAL);
		renderer.DrawVertexArray(xrayVerts);

		renderer.SetBlendMode(BlendMode::OPAQUE);
		renderer.SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
		renderer.DrawVertexArray(verts);
		break;
	}

	case DebugRenderMode::USE_DEPTH:
	default:
		renderer.SetBlendMode(BlendMode::ALPHA);
		renderer.SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
		renderer.DrawVertexArray(verts);
		break;
	}
}

void RenderWorldGeometryObject(DebugObject const& object)
{
	if (!IsSystemReady()) {
		return;
	}

	Renderer& renderer = *g_debugRenderState->m_config.m_renderer;
	Rgba8 tint = GetCurrentColor(object);
	std::vector<Vertex> tintedVerts = object.m_geometryVerts;
	ApplyColorToVerts(tintedVerts, tint);
	DrawWorldVertsByMode(renderer, tintedVerts, Mat44(), object.m_mode, nullptr);
}

void BuildTextVertsAtOrigin(std::vector<Vertex>& outVerts, DebugObject const& object, Rgba8 const& tint)
{
	BitmapFont* font = GetDebugFont();
	if (font == nullptr || object.m_text.empty() || object.m_textHeight <= 0.f) {
		return;
	}

	font->AddVertsForText3DAtOriginXForward(
		outVerts,
		object.m_textHeight,
		object.m_text,
		tint,
		1.f,
		object.m_alignment,
		999);
}

void RenderWorldTextObject(DebugObject const& object, Camera const& camera)
{
	if (!IsSystemReady()) {
		return;
	}

	BitmapFont* font = GetDebugFont();
	if (font == nullptr) {
		return;
	}

	Rgba8 tint = GetCurrentColor(object);
	std::vector<Vertex> textVerts;
	BuildTextVertsAtOrigin(textVerts, object, tint);
	if (textVerts.empty()) {
		return;
	}

	Mat44 textTransform = object.m_transform;
	if (object.m_type == DebugObjectType::WORLD_BILLBOARD_TEXT) {
		textTransform = GetBillboardTransform(
			BillboardType::FULL_OPPOSING,
			camera.GetCameraToWorldTransform(),
			object.m_billboardOrigin);
	}

	Renderer& renderer = *g_debugRenderState->m_config.m_renderer;
	DrawWorldVertsByMode(renderer, textVerts, textTransform, object.m_mode, &font->GetTexture());
}

void RenderScreenTextObject(DebugObject const& object)
{
	if (!IsSystemReady()) {
		return;
	}

	BitmapFont* font = GetDebugFont();
	if (font == nullptr || object.m_text.empty() || object.m_screenCellHeight <= 0.f) {
		return;
	}

	Rgba8 tint = GetCurrentColor(object);
	std::vector<Vertex> verts;
	font->AddVertsForTextInBox2D(
		verts,
		object.m_text,
		object.m_screenBox,
		object.m_screenCellHeight,
		tint,
		1.f,
		object.m_alignment,
		TextBoxMode::SHRINK_TO_FIT,
		999999);

	if (verts.empty()) {
		return;
	}

	Renderer& renderer = *g_debugRenderState->m_config.m_renderer;
	renderer.SetModelConstants(Mat44(), Rgba8::WHITE);
	renderer.BindTexture(&font->GetTexture());
	renderer.SetBlendMode(BlendMode::ALPHA);
	renderer.SetDepthMode(DepthMode::DISABLED);
	renderer.DrawVertexArray(verts);
}

void RenderMessages(Camera const& camera)
{
	if (!IsSystemReady()) {
		return;
	}

	BitmapFont* font = GetDebugFont();
	if (font == nullptr) {
		return;
	}

	std::vector<DebugObject const*> messageObjects;
	for (DebugObject const& object : g_debugRenderState->m_objects) {
		if (object.m_type == DebugObjectType::MESSAGE) {
			messageObjects.push_back(&object);
		}
	}

	if (messageObjects.empty()) {
		return;
	}

	std::stable_sort(
		messageObjects.begin(),
		messageObjects.end(),
		[](DebugObject const* a, DebugObject const* b)
		{
			if (a->m_isInfinite != b->m_isInfinite) {
				return a->m_isInfinite && !b->m_isInfinite;
			}
			return a->m_addOrder < b->m_addOrder;
		});

	Vec2 const screenMins = camera.GetOrthographicBottomLeft();
	Vec2 const screenMaxs = camera.GetOrthographicTopRight();
	float const textLeft = screenMins.x + DEBUG_MESSAGE_MARGIN;
	float const textRight = screenMaxs.x - DEBUG_MESSAGE_MARGIN;
	// Reserve the very top line for per-frame HUD text (e.g. player position).
	float lineTop = screenMaxs.y - DEBUG_MESSAGE_MARGIN - DEBUG_MESSAGE_LINE_HEIGHT - DEBUG_MESSAGE_LINE_SPACING;

	Renderer& renderer = *g_debugRenderState->m_config.m_renderer;
	renderer.SetModelConstants(Mat44(), Rgba8::WHITE);
	renderer.BindTexture(&font->GetTexture());
	renderer.SetBlendMode(BlendMode::ALPHA);
	renderer.SetDepthMode(DepthMode::DISABLED);

	int const messageCount = static_cast<int>(messageObjects.size());
	for (int messageIndex = 0; messageIndex < messageCount; ++messageIndex) {
		DebugObject const& message = *messageObjects[messageIndex];
		Rgba8 tint = GetCurrentColor(message);

		float top = lineTop - static_cast<float>(messageIndex) * (DEBUG_MESSAGE_LINE_HEIGHT + DEBUG_MESSAGE_LINE_SPACING);
		float bottom = top - DEBUG_MESSAGE_LINE_HEIGHT;
		AABB2 lineBox(textLeft, bottom, textRight, top);

		std::vector<Vertex> verts;
		font->AddVertsForTextInBox2D(
			verts,
			message.m_text,
			lineBox,
			DEBUG_MESSAGE_LINE_HEIGHT,
			tint,
			1.f,
			Vec2(0.f, 1.f),
			TextBoxMode::OVERRUN,
			999999);
		renderer.DrawVertexArray(verts);
	}
}
}

void DebugRenderSystemStartup(const DebugRenderConfig& config)
{
	DebugRenderSystemShutdown();

	g_debugRenderState = new DebugRenderSystemState();
	g_debugRenderState->m_config = config;
	g_debugRenderState->m_isVisible = true;
	g_debugRenderState->m_nextAddOrder = 1u;

	g_debugRenderState->m_fontPathWithName = config.m_fontPath;
	if (!g_debugRenderState->m_fontPathWithName.empty()) {
		char const lastChar = g_debugRenderState->m_fontPathWithName[g_debugRenderState->m_fontPathWithName.size() - 1];
		if (lastChar != '/' && lastChar != '\\') {
			g_debugRenderState->m_fontPathWithName += "/";
		}
	}
	g_debugRenderState->m_fontPathWithName += config.m_fontName;

	SubscribeEventCallbackFunction(DEBUG_COMMAND_CLEAR, Command_DebugRenderClear);
	SubscribeEventCallbackFunction(DEBUG_COMMAND_TOGGLE, Command_DebugRenderToggle);
	SubscribeEventCallbackFunction(DEBUG_COMMAND_CLEAR_ALIAS, Command_DebugRenderClear);
	SubscribeEventCallbackFunction(DEBUG_COMMAND_TOGGLE_ALIAS, Command_DebugRenderToggle);
}

void DebugRenderSystemShutdown()
{
	if (g_debugRenderState == nullptr) {
		return;
	}

	UnsubscribeEventCallbackFunction(DEBUG_COMMAND_CLEAR, Command_DebugRenderClear);
	UnsubscribeEventCallbackFunction(DEBUG_COMMAND_TOGGLE, Command_DebugRenderToggle);
	UnsubscribeEventCallbackFunction(DEBUG_COMMAND_CLEAR_ALIAS, Command_DebugRenderClear);
	UnsubscribeEventCallbackFunction(DEBUG_COMMAND_TOGGLE_ALIAS, Command_DebugRenderToggle);

	delete g_debugRenderState;
	g_debugRenderState = nullptr;
}

void DebugRenderSetVisible()
{
	if (g_debugRenderState == nullptr) {
		return;
	}

	g_debugRenderState->m_isVisible = true;
}

void DebugRenderSetHidden()
{
	if (g_debugRenderState == nullptr) {
		return;
	}

	g_debugRenderState->m_isVisible = false;
}

void DebugRenderClear()
{
	if (g_debugRenderState == nullptr) {
		return;
	}

	g_debugRenderState->m_objects.clear();
}

void DebugRenderBeginFrame()
{
	RemoveExpiredFiniteObjects();
}

void DebugRenderWorld(const Camera& camera)
{
	if (!IsSystemReady() || !g_debugRenderState->m_isVisible) {
		return;
	}

	for (DebugObject const& object : g_debugRenderState->m_objects) {
		switch (object.m_type)
		{
		case DebugObjectType::WORLD_GEOMETRY:
			RenderWorldGeometryObject(object);
			break;

		case DebugObjectType::WORLD_TEXT:
		case DebugObjectType::WORLD_BILLBOARD_TEXT:
			RenderWorldTextObject(object, camera);
			break;

		case DebugObjectType::SCREEN_TEXT:
		case DebugObjectType::MESSAGE:
		default:
			break;
		}
	}
}

void DebugRenderScreen(const Camera& camera)
{
	if (!IsSystemReady() || !g_debugRenderState->m_isVisible) {
		return;
	}

	for (DebugObject const& object : g_debugRenderState->m_objects) {
		if (object.m_type == DebugObjectType::SCREEN_TEXT) {
			RenderScreenTextObject(object);
		}
	}

	RenderMessages(camera);
}

void DebugRenderEndFrame()
{
	RemoveOneFrameObjects();
}

void DebugAddWorldSphere(const Vec3& center, float radius, float duration,
	const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	DebugObject object = CreateDebugObject(DebugObjectType::WORLD_GEOMETRY, duration, startColor, endColor, mode);
	AddVertsForSphere3D(object.m_geometryVerts, center, radius, Rgba8::WHITE, AABB2(0.f, 0.f, 1.f, 1.f), 32, 16);
	AddObject(std::move(object));
}

void DebugAddWorldWireSphere(const Vec3& center, float radius, float duration,
	const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	DebugObject object = CreateDebugObject(DebugObjectType::WORLD_GEOMETRY, duration, startColor, endColor, mode);
	AddWireSphere3D(object.m_geometryVerts, center, radius, Rgba8::WHITE);
	AddObject(std::move(object));
}

void DebugAddWorldCylinder(const Vec3& start, const Vec3& end, float radius, float duration,
	const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	DebugObject object = CreateDebugObject(DebugObjectType::WORLD_GEOMETRY, duration, startColor, endColor, mode);
	AddVertsForCylinder3D(object.m_geometryVerts, start, end, radius, Rgba8::WHITE, AABB2(0.f, 0.f, 1.f, 1.f), 32);
	AddObject(std::move(object));
}

void DebugAddWorldWireCylinder(const Vec3& start, const Vec3& end, float radius, float duration,
	const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	DebugObject object = CreateDebugObject(DebugObjectType::WORLD_GEOMETRY, duration, startColor, endColor, mode);
	AddWireCylinder3D(object.m_geometryVerts, start, end, radius, Rgba8::WHITE);
	AddObject(std::move(object));
}

void DebugAddWorldArrow(const Vec3& start, const Vec3& end, float radius, float duration,
	const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	DebugObject object = CreateDebugObject(DebugObjectType::WORLD_GEOMETRY, duration, startColor, endColor, mode);
	AddVertsForArrow3D(object.m_geometryVerts, start, end, radius, Rgba8::WHITE, 32);
	AddObject(std::move(object));
}

void DebugAddWorldWireArrow(const Vec3& start, const Vec3& end, float radius, float duration,
	const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	DebugObject object = CreateDebugObject(DebugObjectType::WORLD_GEOMETRY, duration, startColor, endColor, mode);
	AddWireArrow3D(object.m_geometryVerts, start, end, radius, Rgba8::WHITE);
	AddObject(std::move(object));
}

void DebugAddBasis(const Mat44& transform, float duration, float length, float radius,
	float colorScale, float alphaScale, DebugRenderMode mode)
{
	Vec3 const origin = transform.GetTranslation3D();
	Vec3 const iBasis = GetNormalizedOrFallback(transform.GetIBasis3D(), Vec3(1.f, 0.f, 0.f));
	Vec3 const jBasis = GetNormalizedOrFallback(transform.GetJBasis3D(), Vec3(0.f, 1.f, 0.f));
	Vec3 const kBasis = GetNormalizedOrFallback(transform.GetKBasis3D(), Vec3(0.f, 0.f, 1.f));

	DebugAddWorldArrow(
		origin,
		origin + (iBasis * length),
		radius,
		duration,
		ScaleColor(Rgba8::RED, colorScale, alphaScale),
		ScaleColor(Rgba8::RED, colorScale, alphaScale),
		mode);
	DebugAddWorldArrow(
		origin,
		origin + (jBasis * length),
		radius,
		duration,
		ScaleColor(Rgba8::GREEN, colorScale, alphaScale),
		ScaleColor(Rgba8::GREEN, colorScale, alphaScale),
		mode);
	DebugAddWorldArrow(
		origin,
		origin + (kBasis * length),
		radius,
		duration,
		ScaleColor(Rgba8::BLUE, colorScale, alphaScale),
		ScaleColor(Rgba8::BLUE, colorScale, alphaScale),
		mode);
}

void DebugAddWorldBasis(const Mat44& transform, float duration, DebugRenderMode mode)
{
	DebugAddBasis(transform, duration, 4.f, 0.05f, 1.f, 1.f, mode);
}

void DebugAddWorldText(const std::string& text, const Mat44& transform, float textHeight,
	const Vec2& alignment, float duration,
	const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	DebugObject object = CreateDebugObject(DebugObjectType::WORLD_TEXT, duration, startColor, endColor, mode);
	object.m_text = text;
	object.m_textHeight = textHeight;
	object.m_alignment = alignment;
	object.m_transform = transform;
	AddObject(std::move(object));
}

void DebugAddWorldBillboardText(const std::string& text, const Vec3& origin, float textHeight,
	const Vec2& alignment, float duration,
	const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	DebugObject object = CreateDebugObject(DebugObjectType::WORLD_BILLBOARD_TEXT, duration, startColor, endColor, mode);
	object.m_text = text;
	object.m_textHeight = textHeight;
	object.m_alignment = alignment;
	object.m_billboardOrigin = origin;
	AddObject(std::move(object));
}

void DebugAddScreenText(const std::string& text, const AABB2& box, float cellHeight,
	const Vec2& alignment, float duration,
	const Rgba8& startColor, const Rgba8& endColor)
{
	DebugObject object = CreateDebugObject(DebugObjectType::SCREEN_TEXT, duration, startColor, endColor, DebugRenderMode::ALWAYS);
	object.m_text = text;
	object.m_screenBox = box;
	object.m_screenCellHeight = cellHeight;
	object.m_alignment = alignment;
	AddObject(std::move(object));
}

void DebugAddMessage(const std::string& text, float duration,
	const Rgba8& startColor, const Rgba8& endColor)
{
	DebugObject object = CreateDebugObject(DebugObjectType::MESSAGE, duration, startColor, endColor, DebugRenderMode::ALWAYS);
	object.m_text = text;
	AddObject(std::move(object));
}

bool Command_DebugRenderClear([[maybe_unused]] EventArgs& args)
{
	DebugRenderClear();
	return true;
}

bool Command_DebugRenderToggle([[maybe_unused]] EventArgs& args)
{
	if (g_debugRenderState == nullptr) {
		return false;
	}

	if (g_debugRenderState->m_isVisible) {
		DebugRenderSetHidden();
	}
	else {
		DebugRenderSetVisible();
	}

	return true;
}

