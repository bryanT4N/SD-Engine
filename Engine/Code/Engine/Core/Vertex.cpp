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

Vertex_PCUTBN::Vertex_PCUTBN(
	Vec3 const& position,
	Rgba8 const& color,
	Vec2 const& uvTexCoords,
	Vec3 const& tangent,
	Vec3 const& bitangent,
	Vec3 const& normal)
	: m_position(position)
	, m_color(color)
	, m_uvTexCoords(uvTexCoords)
	, m_tangent(tangent)
	, m_bitangent(bitangent)
	, m_normal(normal)
{
}
