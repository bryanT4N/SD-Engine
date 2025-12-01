#include "Engine/Core/TileHeatMap.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
TileHeatMap::TileHeatMap( IntVec2 const& dimensions )
	: m_dimensions( dimensions )
{
	int numTiles = m_dimensions.x * m_dimensions.y;
	GUARANTEE_OR_DIE( numTiles > 0, "TileHeatMap dimensions must be positive." );

	m_values = new float[numTiles];

	SetAllValues( 0.0f );
}

//-----------------------------------------------------------------------------------------------
TileHeatMap::~TileHeatMap()
{
	delete[] m_values;
	m_values = nullptr;
}

//-----------------------------------------------------------------------------------------------
IntVec2 TileHeatMap::GetDimensions() const
{
	return m_dimensions;
}

//-----------------------------------------------------------------------------------------------
void TileHeatMap::SetAllValues( float value )
{
	int numTiles = m_dimensions.x * m_dimensions.y;
	for( int index = 0; index < numTiles; ++index )
	{
		m_values[index] = value;
	}
}

//-----------------------------------------------------------------------------------------------
float TileHeatMap::GetValue( IntVec2 const& tileCoords ) const
{
	int index = GetIndexForTileCoords( tileCoords );
	return m_values[index];
}

//-----------------------------------------------------------------------------------------------
void TileHeatMap::SetValue( IntVec2 const& tileCoords, float value )
{
	int index = GetIndexForTileCoords( tileCoords );
	m_values[index] = value;
}

//-----------------------------------------------------------------------------------------------
void TileHeatMap::AddValue( IntVec2 const& tileCoords, float deltaValue )
{
	int index = GetIndexForTileCoords( tileCoords );
	m_values[index] += deltaValue;
}

//-----------------------------------------------------------------------------------------------
void TileHeatMap::AddVertsForDebugDraw(
	std::vector<Vertex>& verts,
	AABB2 totalBounds,
	FloatRange valueRange,
	Rgba8 lowColor,
	Rgba8 highColor,
	float specialValue,
	Rgba8 specialColor )
{
	if( m_values == nullptr )
	{
		return;
	}

	if( m_dimensions.x <= 0 || m_dimensions.y <= 0 )
	{
		return;
	}

	Vec2 totalSize = totalBounds.GetDimensions();
	float tileWidth = totalSize.x / static_cast<float>( m_dimensions.x );
	float tileHeight = totalSize.y / static_cast<float>( m_dimensions.y );

	for( int y = 0; y < m_dimensions.y; ++y )
	{
		for( int x = 0; x < m_dimensions.x; ++x )
		{
			IntVec2 tileCoords( x, y );
			int index = GetIndexForTileCoords( tileCoords );
			float value = m_values[index];

			Rgba8 tileColor;
			if( value == specialValue )
			{
				tileColor = specialColor;
			}
			else
			{
				float t = RangeMapClamped( value, valueRange.m_min, valueRange.m_max, 0.0f, 1.0f );
				t = GetClampedZeroToOne( t );
				tileColor = Interpolate( lowColor, highColor, t );
			}

			Vec2 tileMins;
			tileMins.x = totalBounds.m_mins.x + static_cast<float>( x ) * tileWidth;
			tileMins.y = totalBounds.m_mins.y + static_cast<float>( y ) * tileHeight;

			Vec2 tileMaxs;
			tileMaxs.x = tileMins.x + tileWidth;
			tileMaxs.y = tileMins.y + tileHeight;

			AABB2 tileBounds( tileMins, tileMaxs );
			AddVertsForAABB2D( verts, tileBounds, tileColor );
		}
	}
}

//-----------------------------------------------------------------------------------------------
int TileHeatMap::GetIndexForTileCoords( IntVec2 const& tileCoords ) const
{
	GUARANTEE_OR_DIE( tileCoords.x >= 0 && tileCoords.x < m_dimensions.x, "TileHeatMap x coordinate out of range." );
	GUARANTEE_OR_DIE( tileCoords.y >= 0 && tileCoords.y < m_dimensions.y, "TileHeatMap y coordinate out of range." );

	int index = tileCoords.x + tileCoords.y * m_dimensions.x;
	return index;
}


