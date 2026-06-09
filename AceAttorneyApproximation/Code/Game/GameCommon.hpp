#pragma once

//-----------------------------------------------------------------------------------------------
enum class GameStates {
	INVALID = -1,
	ATTRACT,
	PLAYING,
	NUM,
};

//-----------------------------------------------------------------------------------------------
constexpr float WORLD_SIZE_X						= 200.f;
constexpr float WORLD_SIZE_Y						= 100.f;
constexpr float WORLD_CENTER_X						= WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y						= WORLD_SIZE_Y / 2.f;

constexpr float SCREEN_SIZE_X						= 1600.f;
constexpr float SCREEN_SIZE_Y						= 800.f;
constexpr float SCREEN_CENTER_X						= SCREEN_SIZE_X / 2.f;
constexpr float SCREEN_CENTER_Y						= SCREEN_SIZE_Y / 2.f;

constexpr float ATTRACT_SCREEN_SIZE_X				= 1600.f;
constexpr float ATTRACT_SCREEN_SIZE_Y				= 800.f;
constexpr float ATTRACT_SCREEN_CENTER_X				= ATTRACT_SCREEN_SIZE_X / 2.f;
constexpr float ATTRACT_SCREEN_CENTER_Y				= ATTRACT_SCREEN_SIZE_Y / 2.f;

//-----------------------------------------------------------------------------------------------



// Temporary
struct Vec2;
struct Rgba8;

//-----------------------------------------------------------------------------------------------
void DebugDrawLine(Vec2 const& startPos, Vec2 const& endPos, float const& thickness, Rgba8 const& startColor, Rgba8 const& endColor);
void DebugDrawRing(Vec2 const& originPos, float const& radius, float const& thickness, Rgba8 const& color);
void DrawFadedRing(Vec2 const& originPos, float const& innerRadius, float const& outerRadius, Rgba8 const& innerColor, Rgba8 const& outerColor);
void DrawGlow(Vec2 pos, Rgba8 color, float alpha, float radius);
