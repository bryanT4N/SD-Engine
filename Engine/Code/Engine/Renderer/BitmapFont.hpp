#pragma once

#include <string>
#include <vector>

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

//-----------------------------------------------------------------------------------------------
class Renderer;

//-----------------------------------------------------------------------------------------------
enum class TextBoxMode
{
	SHRINK_TO_FIT,
	OVERRUN,
};

//-----------------------------------------------------------------------------------------------
class BitmapFont
{
	friend class Renderer;

private:
	BitmapFont( char const* fontFilePathNameWithNoExtension, Texture& fontTexture );

public:
	Texture& GetTexture();

	void AddVertsForText2D( std::vector<Vertex>& vertexArray, Vec2 textMins,
		float cellHeight, std::string const& text, Rgba8 tint = Rgba8::WHITE, float cellAspectScale = 1.f );

	void AddVertsForText3DAtOriginXForward(
		std::vector<Vertex>& vertexArray,
		float cellHeight,
		std::string const& text,
		Rgba8 tint = Rgba8::WHITE,
		float cellAspectScale = 1.f,
		Vec2 const& alignment = Vec2(0.5f, 0.5f),
		int maxGlyphsToDraw = 999);

	void AddVertsForTextInBox2D( std::vector<Vertex>& vertexArray, std::string const& text, AABB2 const& box,
		float cellHeight, Rgba8 tint = Rgba8::WHITE, float cellAspectScale = 1.f,
		Vec2 alignment = Vec2( 0.5f, 0.5f ), TextBoxMode mode = TextBoxMode::SHRINK_TO_FIT,
		int maxGlyphsToDraw = 99999999 );

	std::string GetTextWrappedToWidth( std::string const& text, float boxWidth, float cellHeight, float cellAspectScale = 1.f );

	void AddVertsForWrappedTextInBox2D( std::vector<Vertex>& vertexArray, std::string const& text, AABB2 const& box,
		float cellHeight, Rgba8 tint = Rgba8::WHITE, float cellAspectScale = 1.f );

	float GetTextWidth( float cellHeight, std::string const& text, float cellAspectScale = 1.f );

protected:
	float GetGlyphAspect( int glyphUnicode ) const; // For now this will always return m_fontDefaultAspect

protected:
	std::string	m_fontFilePathNameWithNoExtension;
	SpriteSheet	m_fontGlyphsSpriteSheet;
	float		m_fontDefaultAspect = 1.0f; // For basic (tier 1) fonts, set this to the aspect of the sprite sheet texture
};


