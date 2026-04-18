#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"

//-----------------------------------------------------------------------------------------------
struct Vertex {
public:
	Vec3 m_position;
	Rgba8 m_color;
	Vec2 m_uvTexCoords;
	Vec3 m_tangent = Vec3(1.f, 0.f, 0.f);
	Vec3 m_bitangent = Vec3(0.f, 1.f, 0.f);
	Vec3 m_normal = Vec3(0.f, 0.f, 1.f);

	// Construction/Destruction
	~Vertex() {}											// destructor (do nothing)
	Vertex() = default;
	explicit Vertex(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords);
	Vertex(
		Vec3 const& position,
		Rgba8 const& color,
		Vec2 const& uvTexCoords,
		Vec3 const& tangent,
		Vec3 const& bitangent,
		Vec3 const& normal);
};

//-----------------------------------------------------------------------------------------------
typedef Vertex Vertex_PCU;

//-----------------------------------------------------------------------------------------------
typedef Vertex Vertex_PCUTBN;

