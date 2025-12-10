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
void BitmapFont::AddVertsForTextInBox2D( std::vector<Vertex>& vertexArray, std::string const& text, AABB2 const& box,
	float cellHeight, Rgba8 tint, float cellAspectScale, Vec2 alignment, TextBoxMode mode, int maxGlyphsToDraw )
{
	if( cellHeight <= 0.0f ) { return; }
	int textLength = static_cast<int>( text.length() );
	if( textLength == 0 ) { return; }

	// measure raw widths
	float maxLineWidth = 0.0f;
	float currentLineWidth = 0.0f;
	std::vector<float> lineWidths;

	for( int charIndex = 0; charIndex < textLength; ++charIndex ) {
		unsigned char glyphChar = static_cast<unsigned char>( text[charIndex] );

		if( glyphChar == '\n' ) {
			lineWidths.push_back( currentLineWidth );
			if( currentLineWidth > maxLineWidth ) {
				maxLineWidth = currentLineWidth;
			}
			currentLineWidth = 0.0f;
			continue;
		}

		float glyphAspect = GetGlyphAspect( static_cast<int>( glyphChar ) );
		float glyphWidth = cellHeight * cellAspectScale * glyphAspect;
		currentLineWidth += glyphWidth;
	}

	lineWidths.push_back( currentLineWidth );
	if( currentLineWidth > maxLineWidth ) {
		maxLineWidth = currentLineWidth;
	}

	int lineCount = static_cast<int>( lineWidths.size() );
	if( maxLineWidth <= 0.0f || lineCount <= 0 ) {
		return;
	}

	float textHeight = static_cast<float>( lineCount ) * cellHeight;

	Vec2 boxDims = box.GetDimensions();

	// scaleY
	float scaleY = 1.0f;
	if( mode == TextBoxMode::SHRINK_TO_FIT && boxDims.y > 0.0f ) {
		float neededScaleY = boxDims.y / textHeight;
		if( neededScaleY < 1.0f ) { scaleY = neededScaleY; }
	}

	float scaledCellHeight = cellHeight * scaleY;
	float scaledTextHeight = static_cast<float>( lineCount ) * scaledCellHeight;

	// scaleX per line
	std::vector<float> lineScaleX;
	std::vector<float> scaledLineWidths;
	lineScaleX.resize( static_cast<size_t>( lineCount ), 1.0f );
	scaledLineWidths.resize( static_cast<size_t>( lineCount ), 0.0f );

	for( int lineIndex = 0; lineIndex < lineCount; ++lineIndex ) {
		float baseWidth = lineWidths[static_cast<size_t>( lineIndex )] * scaleY;
		float scaleXLine = 1.0f;

		if( mode == TextBoxMode::SHRINK_TO_FIT && boxDims.x > 0.0f && baseWidth > boxDims.x ) {
			scaleXLine = boxDims.x / baseWidth;
		}

		lineScaleX[static_cast<size_t>( lineIndex )] = scaleXLine;
		scaledLineWidths[static_cast<size_t>( lineIndex )] = baseWidth * scaleXLine;
	}

	// block width is the widest after scaling
	float scaledTextWidth = 0.0f;
	for( int lineIndex = 0; lineIndex < lineCount; ++lineIndex ) {
		float w = scaledLineWidths[static_cast<size_t>( lineIndex )];
		if( w > scaledTextWidth ) {
			scaledTextWidth = w;
		}
	}

	// origin alignment
	Vec2 leftover;
	leftover.x = boxDims.x - scaledTextWidth;
	leftover.y = boxDims.y - scaledTextHeight;

	Vec2 blockOrigin;
	blockOrigin.x = box.m_mins.x + leftover.x * alignment.x;
	blockOrigin.y = box.m_mins.y + leftover.y * alignment.y;

	// draw glyphs
	std::vector<float> lineStartsX;
	lineStartsX.reserve( static_cast<size_t>( lineCount ) );

	for( int lineIndex = 0; lineIndex < lineCount; ++lineIndex ) {
		float lineWidthScaled = scaledLineWidths[static_cast<size_t>( lineIndex )];
		float lineLeftover = scaledTextWidth - lineWidthScaled;
		float lineStartX = blockOrigin.x + lineLeftover * alignment.x; // line text horizontal alignment
		lineStartsX.push_back( lineStartX );
	}

	Vec2 penPosition = blockOrigin;

	int glyphsDrawn = 0;
	int currentLineIndex = 0;

	for( int charIndex = 0; charIndex < textLength; ++charIndex ) {
		unsigned char glyphChar = static_cast<unsigned char>( text[charIndex] );

		if( glyphChar == '\n' ) {
			++currentLineIndex;
			if( currentLineIndex >= lineCount ) { break; }
			penPosition.x = lineStartsX[static_cast<size_t>( currentLineIndex )];
			penPosition.y = blockOrigin.y + static_cast<float>( currentLineIndex ) * scaledCellHeight;
			continue;
		}

		if( glyphsDrawn >= maxGlyphsToDraw ) { break; }

		float glyphAspect = GetGlyphAspect( static_cast<int>( glyphChar ) );
		float glyphWidth = scaledCellHeight * cellAspectScale * glyphAspect * lineScaleX[static_cast<size_t>( currentLineIndex )];

		Vec2 glyphMins = penPosition;
		Vec2 glyphMaxs = Vec2( penPosition.x + glyphWidth, penPosition.y + scaledCellHeight );

		Vec2 uvMins;
		Vec2 uvMaxs;
		m_fontGlyphsSpriteSheet.GetSpriteUVs( uvMins, uvMaxs, static_cast<int>( glyphChar ) );

		AABB2 glyphBounds( glyphMins, glyphMaxs );
		AABB2 glyphUVs( uvMins, uvMaxs );
		AddVertsForAABB2D( vertexArray, glyphBounds, tint, glyphUVs );

		penPosition.x += glyphWidth;
		++glyphsDrawn;
	}
}

//-----------------------------------------------------------------------------------------------
float BitmapFont::GetGlyphAspect( int glyphUnicode ) const
{
	(void)glyphUnicode;
	return m_fontDefaultAspect;
}


