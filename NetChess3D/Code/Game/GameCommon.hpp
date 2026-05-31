#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"

//-----------------------------------------------------------------------------------------------
enum class GameStates {
	INVALID = -1,
	ATTRACT,
	PLAYING,
	NUM,
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

//-----------------------------------------------------------------------------------------------
constexpr char const* SCENE_CUBE_TEXTURE_PATH = "Data/Images/Test_StbiFlippedAndOpenGL.png";
constexpr char const* SCENE_SPHERE_TEXTURE_PATH = "Data/Images/TestUV.png";
constexpr float SCENE_CUBE_SIDE_LENGTH = 1.0f;
constexpr float SCENE_PRIMARY_CUBE_ROTATE_SPEED_DEGREES = 30.0f;
constexpr float SCENE_SPHERE_RADIUS = 1.0f;
constexpr int SCENE_SPHERE_NUM_SLICES = 32;
constexpr int SCENE_SPHERE_NUM_STACKS = 16;
constexpr float SCENE_SPHERE_ROTATE_Z_SPEED_DEGREES = 45.0f;
constexpr float SCENE_GRID_HALF_EXTENT = 100.0f;
constexpr float SCENE_GRID_SPACING = 1.0f;
constexpr float SCENE_GRID_MINOR_THICKNESS = 0.06f;
constexpr float SCENE_GRID_MAJOR_THICKNESS = 0.12f;
constexpr float SCENE_GRID_AXIS_THICKNESS = 0.20f;
constexpr unsigned char SCENE_GRID_MINOR_INTENSITY = 96;
constexpr unsigned char SCENE_GRID_MAJOR_INTENSITY = 176;
constexpr unsigned char SCENE_GRID_AXIS_INTENSITY = 255;
constexpr int SCENE_GRID_MAJOR_INTERVAL = 5;
constexpr float ATTRACT_RING_BASE_OUTER_RADIUS = 16.0f;
constexpr float ATTRACT_RING_HALF_PULSE_SPAN = 5.0f;
constexpr float ATTRACT_RING_PULSE_SCALE = 0.1f;
constexpr double ATTRACT_RING_PULSE_TIME_SCALE = 100.0;
constexpr int ATTRACT_RING_PULSE_PERIOD_TICKS = 100;

//-----------------------------------------------------------------------------------------------
class Game;
class Prop;
class Texture;
struct Vec2;

//-----------------------------------------------------------------------------------------------
struct GridPropConfig
{
	GridPropConfig(
		float halfExtent = SCENE_GRID_HALF_EXTENT,
		float spacing = SCENE_GRID_SPACING,
		float minorLineThickness = SCENE_GRID_MINOR_THICKNESS,
		float majorLineThickness = SCENE_GRID_MAJOR_THICKNESS,
		float axisLineThickness = SCENE_GRID_AXIS_THICKNESS,
		unsigned char minorGrayIntensity = SCENE_GRID_MINOR_INTENSITY,
		unsigned char majorRedGreenIntensity = SCENE_GRID_MAJOR_INTENSITY,
		unsigned char axisIntensity = SCENE_GRID_AXIS_INTENSITY,
		int majorLineInterval = SCENE_GRID_MAJOR_INTERVAL)
		: m_halfExtent(halfExtent)
		, m_spacing(spacing)
		, m_minorLineThickness(minorLineThickness)
		, m_majorLineThickness(majorLineThickness)
		, m_axisLineThickness(axisLineThickness)
		, m_minorGrayIntensity(minorGrayIntensity)
		, m_majorRedGreenIntensity(majorRedGreenIntensity)
		, m_axisIntensity(axisIntensity)
		, m_majorLineInterval(majorLineInterval)
	{
	}

	float m_halfExtent = SCENE_GRID_HALF_EXTENT;
	float m_spacing = SCENE_GRID_SPACING;
	float m_minorLineThickness = SCENE_GRID_MINOR_THICKNESS;
	float m_majorLineThickness = SCENE_GRID_MAJOR_THICKNESS;
	float m_axisLineThickness = SCENE_GRID_AXIS_THICKNESS;
	unsigned char m_minorGrayIntensity = SCENE_GRID_MINOR_INTENSITY;
	unsigned char m_majorRedGreenIntensity = SCENE_GRID_MAJOR_INTENSITY;
	unsigned char m_axisIntensity = SCENE_GRID_AXIS_INTENSITY;
	int m_majorLineInterval = SCENE_GRID_MAJOR_INTERVAL;
};

//-----------------------------------------------------------------------------------------------
Prop* CreateCubeProp(
	Game* owner,
	float sideLength = SCENE_CUBE_SIDE_LENGTH,
	Texture* texture = nullptr,
	AABB2 const& uvs = AABB2(0.0f, 0.0f, 1.0f, 1.0f),
	Rgba8 const& positiveXColor = Rgba8(255, 0, 0, 255),
	Rgba8 const& negativeXColor = Rgba8(0, 255, 255, 255),
	Rgba8 const& positiveYColor = Rgba8(0, 255, 0, 255),
	Rgba8 const& negativeYColor = Rgba8(255, 0, 255, 255),
	Rgba8 const& positiveZColor = Rgba8(0, 0, 255, 255),
	Rgba8 const& negativeZColor = Rgba8(255, 255, 0, 255));

Prop* CreateSphereProp(
	Game* owner,
	float radius = SCENE_SPHERE_RADIUS,
	int numSlices = SCENE_SPHERE_NUM_SLICES,
	int numStacks = SCENE_SPHERE_NUM_STACKS,
	Texture* texture = nullptr,
	AABB2 const& uvs = AABB2(0.0f, 0.0f, 1.0f, 1.0f),
	Rgba8 const& color = Rgba8::WHITE);
Prop* CreateGridProp(Game* owner, GridPropConfig const& config = GridPropConfig());

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
