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

	// Construction/Destruction
	~Vertex() {}											// destructor (do nothing)
	Vertex() {}												// default constructor (do nothing)
	explicit Vertex(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords);
};

//-----------------------------------------------------------------------------------------------
typedef Vertex Vertex_PCU;

