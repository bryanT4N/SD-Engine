#pragma once

#include <vector>

#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"

//-----------------------------------------------------------------------------------------------
class TileHeatMap
{
private:
	IntVec2 m_dimensions;
	float* m_values = nullptr;

public:
	TileHeatMap( IntVec2 const& dimensions );
	~TileHeatMap();

	IntVec2 GetDimensions() const;

	void	SetAllValues( float value );
	float	GetValue( IntVec2 const& tileCoords ) const;
	void	SetValue( IntVec2 const& tileCoords, float value );
	void	AddValue( IntVec2 const& tileCoords, float deltaValue );

	void	AddVertsForDebugDraw(
		std::vector<Vertex>& verts,
		AABB2 totalBounds,
		FloatRange valueRange = FloatRange( 0.0f, 1.0f ),
		Rgba8 lowColor = Rgba8( 0, 0, 0, 240 ),
		Rgba8 highColor = Rgba8( 255, 255, 255, 240 ),
		float specialValue = 999999.0f,
		Rgba8 specialColor = Rgba8( 0, 0, 255, 240 ) );

private:
	int		GetIndexForTileCoords( IntVec2 const& tileCoords ) const;
};


