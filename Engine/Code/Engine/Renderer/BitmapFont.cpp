#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/VertexUtils.hpp"

//-----------------------------------------------------------------------------------------------
BitmapFont::BitmapFont( char const* fontFilePathNameWithNoExtension, Texture& fontTexture )
	: m_fontFilePathNameWithNoExtension( fontFilePathNameWithNoExtension )
	, m_fontGlyphsSpriteSheet( fontTexture, IntVec2( 16, 16 ) )
{
	IntVec2 textureDimensions = fontTexture.GetDimensions();
	if( textureDimensions.y > 0 ) {
		m_fontDefaultAspect = static_cast<float>( textureDimensions.x ) / static_cast<float>( textureDimensions.y );
	}
}

//-----------------------------------------------------------------------------------------------
Texture& BitmapFont::GetTexture()
{
	return m_fontGlyphsSpriteSheet.GetTexture();
}

//-----------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForText2D( std::vector<Vertex>& vertexArray, Vec2 textMins,
	float cellHeight, std::string const& text, Rgba8 tint, float cellAspectScale )
{
	if( cellHeight <= 0.0f ) { return; }

	Vec2 penPosition = textMins;

	int textLength = static_cast<int>( text.length() );
	for( int charIndex = 0; charIndex < textLength; ++charIndex )
	{
		unsigned char glyphChar = static_cast<unsigned char>( text[charIndex] );

		if( glyphChar == '\n' )
		{
			penPosition.x = textMins.x;
			penPosition.y += cellHeight;
			continue;
		}

		float glyphAspect = GetGlyphAspect( static_cast<int>( glyphChar ) );
		float glyphWidth = cellHeight * cellAspectScale * glyphAspect;

		Vec2 glyphMins = penPosition;
		Vec2 glyphMaxs = Vec2( penPosition.x + glyphWidth, penPosition.y + cellHeight );

		Vec2 uvMins;
		Vec2 uvMaxs;
		m_fontGlyphsSpriteSheet.GetSpriteUVs( uvMins, uvMaxs, static_cast<int>( glyphChar ) );

		AABB2 glyphBounds( glyphMins, glyphMaxs );
		AABB2 glyphUVs( uvMins, uvMaxs );
		AddVertsForAABB2D( vertexArray, glyphBounds, tint, glyphUVs );

		penPosition.x += glyphWidth;
	}
}

//-----------------------------------------------------------------------------------------------
float BitmapFont::GetTextWidth( float cellHeight, std::string const& text, float cellAspectScale )
{
	if( cellHeight <= 0.0f )
	{
		return 0.0f;
	}

	float maxLineWidth = 0.0f;
	float currentLineWidth = 0.0f;

	int textLength = static_cast<int>( text.length() );
	for( int charIndex = 0; charIndex < textLength; ++charIndex )
	{
		unsigned char glyphChar = static_cast<unsigned char>( text[charIndex] );

		if( glyphChar == '\n' )
		{
			if( currentLineWidth > maxLineWidth )
			{
				maxLineWidth = currentLineWidth;
			}
			currentLineWidth = 0.0f;
			continue;
		}

		float glyphAspect = GetGlyphAspect( static_cast<int>( glyphChar ) );
		float glyphWidth = cellHeight * cellAspectScale * glyphAspect;
		currentLineWidth += glyphWidth;
	}

	if( currentLineWidth > maxLineWidth )
	{
		maxLineWidth = currentLineWidth;
	}

	return maxLineWidth;
}

//-----------------------------------------------------------------------------------------------
float BitmapFont::GetGlyphAspect( int glyphUnicode ) const
{
	(void)glyphUnicode;
	return m_fontDefaultAspect;
}


