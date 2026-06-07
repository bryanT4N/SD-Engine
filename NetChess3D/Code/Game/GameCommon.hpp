#pragma once
#include "Engine/Core/Rgba8.hpp"

//-----------------------------------------------------------------------------------------------
enum class GameStates {
	INVALID = -1,
	ATTRACT,
	PLAYING,
	NUM,
};

enum class CameraMode {
	POV,
	FREE_SPECTATOR,
	NUM
};

//-----------------------------------------------------------------------------------------------
constexpr float WORLD_SIZE_X = 200.f;
constexpr float WORLD_SIZE_Y = 100.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;

constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;
constexpr float SCREEN_CENTER_X = SCREEN_SIZE_X / 2.f;
constexpr float SCREEN_CENTER_Y = SCREEN_SIZE_Y / 2.f;

constexpr float ATTRACT_SCREEN_SIZE_X = 1600.f;
constexpr float ATTRACT_SCREEN_SIZE_Y = 800.f;
constexpr float ATTRACT_SCREEN_CENTER_X = ATTRACT_SCREEN_SIZE_X / 2.f;
constexpr float ATTRACT_SCREEN_CENTER_Y = ATTRACT_SCREEN_SIZE_Y / 2.f;

constexpr float ATTRACT_RING_BASE_OUTER_RADIUS = 16.0f;
constexpr float ATTRACT_RING_HALF_PULSE_SPAN = 5.0f;
constexpr float ATTRACT_RING_PULSE_SCALE = 0.1f;
constexpr double ATTRACT_RING_PULSE_TIME_SCALE = 100.0;
constexpr int ATTRACT_RING_PULSE_PERIOD_TICKS = 100;

//-----------------------------------------------------------------------------------------------
constexpr float BOARD_SQUARE_SIZE = 1.0f;
constexpr float BOARD_MARGIN_SIZE = 0.333f;
constexpr float BOARD_THICKNESS = 0.333f;
constexpr float BOARD_TILE_RECESS_DEPTH = 0.08f;
inline Rgba8 const LIGHT_SQUARE_COLOR(240, 230, 210, 255);
inline Rgba8 const DARK_SQUARE_COLOR(45, 35, 25, 255);
inline Rgba8 const BOARD_FRAME_COLOR(70, 45, 25, 255);
inline Rgba8 const PLAYER_TINT_GREEN(64, 255, 64, 255);
inline Rgba8 const PLAYER_TINT_RED(255, 64, 64, 255);

//-----------------------------------------------------------------------------------------------
struct Vec2;

//-----------------------------------------------------------------------------------------------
void DebugDrawLine(
	Vec2 const& startPos,
	Vec2 const& endPos,
	float const& thickness,
	Rgba8 const& startColor,
	Rgba8 const& endColor);
void DebugDrawRing(Vec2 const& originPos, float const& radius, float const& thickness, Rgba8 const& color);
void DrawFadedRing(
	Vec2 const& originPos,
	float const& innerRadius,
	float const& outerRadius,
	Rgba8 const& innerColor,
	Rgba8 const& outerColor);
void DrawGlow(Vec2 pos, Rgba8 color, float alpha, float radius);
