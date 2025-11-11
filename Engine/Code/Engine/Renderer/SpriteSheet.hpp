#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

#include <vector>

//-----------------------------------------------------------------------------------------------
struct Vec2;
struct IntVec2;
class AABB2;

//-----------------------------------------------------------------------------------------------
class SpriteSheet{
	friend class SpriteDefinition;

public:
	explicit SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout);

	Texture&						GetTexture() const;
	int								GetNumSprites()const;
	SpriteDefinition const&			GetSpriteDef(int spriteIndex) const; 
	void							GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const; 
	AABB2							GetSpriteUVs(int spriteIndex) const;

protected:
	Texture&						m_texture;			// reference members must be set in constructor's initializer list
	std::vector<SpriteDefinition>	m_spriteDefs;
};