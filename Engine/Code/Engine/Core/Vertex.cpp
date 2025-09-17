#include "Vertex.hpp"
//#include "Engine/Math/MathUtils.hpp"
//#include "Engine/Core/EngineCommon.hpp"

//-----------------------------------------------------------------------------------------------

Vertex::Vertex(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords)
	: m_position(position)
	, m_color(color)
	, m_uvTexCoords(uvTexCoords)
{
}
