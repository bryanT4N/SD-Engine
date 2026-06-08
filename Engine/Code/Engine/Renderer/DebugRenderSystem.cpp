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
	bool m_isWireframe = false;

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

constexpr float DEBUG_MESSAGE_MARGIN = 8.f;
constexpr float DEBUG_MESSAGE_LINE_HEIGHT = 10.f;
constexpr float DEBUG_MESSAGE_LINE_SPACING = 2.f;

constexpr char const* DEBUG_COMMAND_CLEAR = "DebugRenderClear";
constexpr char const* DEBUG_COMMAND_TOGGLE = "DebugRenderToggle";

DebugRenderSystemState* g_debugRenderState = nullptr;

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

bool CompareDebugMessageObjects(DebugObject const* a, DebugObject const* b)
{
	if (a->m_isInfinite != b->m_isInfinite) {
		return a->m_isInfinite && !b->m_isInfinite;
	}

	return a->m_addOrder < b->m_addOrder;
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
	for (size_t objectIndex = 0; objectIndex < objects.size(); ) {
		DebugObject& object = objects[objectIndex];
		if (object.m_isInfinite || object.m_isOneFrame) {
			++objectIndex;
			continue;
		}

		object.m_elapsedSeconds += deltaSeconds;
		if (object.m_elapsedSeconds >= object.m_durationSeconds) {
			objects.erase(objects.begin() + static_cast<long>(objectIndex));
		}
		else {
			++objectIndex;
		}
	}
}

void RemoveOneFrameObjects()
{
	if (!IsSystemReady()) {
		return;
	}

	std::vector<DebugObject>& objects = g_debugRenderState->m_objects;
	for (size_t objectIndex = 0; objectIndex < objects.size(); ) {
		if (objects[objectIndex].m_isOneFrame) {
			objects.erase(objects.begin() + static_cast<long>(objectIndex));
		}
		else {
			++objectIndex;
		}
	}
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
	Texture const* texture,
	bool isWireframe)
{
	if (verts.empty()) {
		return;
	}

	renderer.SetModelCBO(modelTransform, Rgba8::WHITE);
	renderer.BindTexture(texture);
	renderer.SetRasterizerMode(
		isWireframe ? RasterizerMode::WIREFRAME_CULL_BACK : RasterizerMode::SOLID_CULL_BACK);

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
		renderer.SetDepthMode(DepthMode::READ_ONLY_ALWAYS);
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

	renderer.SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
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
	DrawWorldVertsByMode(renderer, tintedVerts, Mat44(), object.m_mode, nullptr, object.m_isWireframe);
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
	renderer.SetModelCBO(textTransform, Rgba8::WHITE);
	renderer.BindTexture(&font->GetTexture());
	renderer.SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);

	switch (object.m_mode)
	{
	case DebugRenderMode::ALWAYS:
		renderer.SetBlendMode(BlendMode::ALPHA);
		renderer.SetDepthMode(DepthMode::DISABLED);
		renderer.DrawVertexArray(textVerts);
		break;

	case DebugRenderMode::XRAY:
	{
		std::vector<Vertex> xrayVerts = textVerts;
		for (Vertex& vert : xrayVerts) {
			Rgba8 xrayColor = Interpolate(vert.m_color, Rgba8::WHITE, 0.5f);
			xrayColor.ScaleAlpha(0.5f);
			vert.m_color = xrayColor;
		}

		renderer.SetBlendMode(BlendMode::ALPHA);
		renderer.SetDepthMode(DepthMode::READ_ONLY_ALWAYS);
		renderer.DrawVertexArray(xrayVerts);

		renderer.SetBlendMode(BlendMode::OPAQUE);
		renderer.SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
		renderer.DrawVertexArray(textVerts);
		break;
	}

	case DebugRenderMode::USE_DEPTH:
	default:
		renderer.SetBlendMode(BlendMode::ALPHA);
		renderer.SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
		renderer.DrawVertexArray(textVerts);
		break;
	}

	renderer.SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
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
		TextBoxMode::OVERRUN,
		999999);

	if (verts.empty()) {
		return;
	}

	Renderer& renderer = *g_debugRenderState->m_config.m_renderer;
	renderer.SetModelCBO(Mat44(), Rgba8::WHITE);
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
		CompareDebugMessageObjects);

	Vec2 const screenMins = camera.GetOrthographicBottomLeft();
	Vec2 const screenMaxs = camera.GetOrthographicTopRight();
	float const textLeft = screenMins.x + DEBUG_MESSAGE_MARGIN;
	float const textRight = screenMaxs.x - DEBUG_MESSAGE_MARGIN;
	// Reserve the very top line for per-frame HUD text (e.g. player position).
	float lineTop = screenMaxs.y - DEBUG_MESSAGE_MARGIN - DEBUG_MESSAGE_LINE_HEIGHT - DEBUG_MESSAGE_LINE_SPACING;

	Renderer& renderer = *g_debugRenderState->m_config.m_renderer;
	renderer.SetModelCBO(Mat44(), Rgba8::WHITE);
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
}

void DebugRenderSystemShutdown()
{
	if (g_debugRenderState == nullptr) {
		return;
	}

	UnsubscribeEventCallbackFunction(DEBUG_COMMAND_CLEAR, Command_DebugRenderClear);
	UnsubscribeEventCallbackFunction(DEBUG_COMMAND_TOGGLE, Command_DebugRenderToggle);

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
	object.m_isWireframe = true;
	AddVertsForSphere3D(
		object.m_geometryVerts,
		center,
		radius,
		Rgba8::WHITE,
		AABB2(0.f, 0.f, 1.f, 1.f),
		32,
		16);
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
	object.m_isWireframe = true;
	AddVertsForCylinder3D(
		object.m_geometryVerts,
		start,
		end,
		radius,
		Rgba8::WHITE,
		AABB2(0.f, 0.f, 1.f, 1.f),
		32);
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
	object.m_isWireframe = true;
	AddVertsForArrow3D(object.m_geometryVerts, start, end, radius, Rgba8::WHITE, 32);
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

void DebugAddWorldBasis()
{
	DebugAddBasis(Mat44(), -1.f, 1.0f, 0.12f, 1.f, 1.f, DebugRenderMode::USE_DEPTH);

	Mat44 xTextTransform;
	xTextTransform.SetIJKT3D( Vec3(0.f, 1.f, 0.f), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(0.24f, 0.f, 0.32f));
	DebugAddWorldText( "x - forward", xTextTransform, 0.20f, Vec2(0.f, 0.5f), -1.f, Rgba8::RED, Rgba8::RED, DebugRenderMode::USE_DEPTH);

	Mat44 yTextTransform;
	yTextTransform.SetIJKT3D( Vec3(1.f, 0.f, 0.f), Vec3(0.f, -1.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(0.f, 1.8f, 0.32f));
	DebugAddWorldText( "y - left", yTextTransform, 0.20f, Vec2(0.f, 0.5f), -1.f, Rgba8::GREEN, Rgba8::GREEN, DebugRenderMode::USE_DEPTH);

	Mat44 zTextTransform;
	zTextTransform.SetIJKT3D( Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(0.f, 1.f, 0.f), Vec3(0.f, -0.32f, 0.24f));
	DebugAddWorldText( "z - up", zTextTransform, 0.20f, Vec2(0.f, 0.5f), -1.f, Rgba8::BLUE, Rgba8::BLUE, DebugRenderMode::USE_DEPTH);
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

