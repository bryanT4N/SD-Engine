#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"

//-----------------------------------------------------------------------------------------------
SpriteSheet::SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout)
	:m_texture(texture)
{
	int numSprites = simpleGridLayout.x * simpleGridLayout.y;
	m_spriteDefs.reserve(numSprites);
	for (int spriteIndex = 0; spriteIndex < numSprites; spriteIndex++) {
		Vec2 uvMins = Vec2::ZERO; Vec2 uvMaxs = Vec2::ONE;
		m_spriteDefs.emplace_back(*this, spriteIndex, uvMins, uvMaxs);
	}

	for (int spriteY = 0; spriteY < simpleGridLayout.y; ++spriteY) {
		for (int spriteX = 0; spriteX < simpleGridLayout.x; ++spriteX) {
			int spriteIndex = spriteX + ((simpleGridLayout.y - spriteY - 1) * simpleGridLayout.x);
			SpriteDefinition& spriteDef = m_spriteDefs[spriteIndex];
			spriteDef.m_uvAtMins.x = static_cast<float>(spriteX + 0) / static_cast<float>(simpleGridLayout.x);
			spriteDef.m_uvAtMaxs.x = static_cast<float>(spriteX + 1) / static_cast<float>(simpleGridLayout.x);
			spriteDef.m_uvAtMins.y = static_cast<float>(spriteY + 0) / static_cast<float>(simpleGridLayout.y);
			spriteDef.m_uvAtMaxs.y = static_cast<float>(spriteY + 1) / static_cast<float>(simpleGridLayout.y);
		}
	}

// 	for (int spriteY = 0; spriteY < simpleGridLayout.y; ++spriteY) {
// 		for (int spriteX = 0; spriteX < simpleGridLayout.x; ++spriteX) {
// 			int spriteIndex = spriteX + (spriteY * simpleGridLayout.x);
// 			SpriteDefinition& spriteDef = m_spriteDefs[spriteIndex];
// 			spriteDef.m_uvAtMins.x = static_cast<float>(spriteX + 0) / static_cast<float>(simpleGridLayout.x);
// 			spriteDef.m_uvAtMins.y = static_cast<float>(spriteY + 0) / static_cast<float>(simpleGridLayout.y);
// 			spriteDef.m_uvAtMaxs.x = static_cast<float>(spriteX + 1) / static_cast<float>(simpleGridLayout.x);
// 			spriteDef.m_uvAtMaxs.y = static_cast<float>(spriteY + 1) / static_cast<float>(simpleGridLayout.y);
// 		}
// 	}
}

Texture& SpriteSheet::GetTexture() const {
	return m_texture;
}

int SpriteSheet::GetNumSprites() const {
	return static_cast<int>(m_spriteDefs.size());
}

SpriteDefinition const& SpriteSheet::GetSpriteDef(int spriteIndex) const {
	return m_spriteDefs[spriteIndex];
}

//-----------------------------------------------------------------------------------------------
AABB2 SpriteSheet::GetSpriteUVs(int spriteIndex) const {
	return m_spriteDefs[spriteIndex].GetUVs();
}

//-----------------------------------------------------------------------------------------------
void SpriteSheet::GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const {
	m_spriteDefs[spriteIndex].GetUVs(out_uvAtMins, out_uvAtMaxs);
}


//-----------------------------------------------------------------------------------------------
// Sprite Definition
//-----------------------------------------------------------------------------------------------
SpriteDefinition::SpriteDefinition(SpriteSheet const& spriteSheet, int spriteIndex, Vec2 const& uvAtMins, Vec2 const& uvAtMaxs)
	: m_spriteSheet(spriteSheet), m_spriteIndex(spriteIndex), m_uvAtMins(uvAtMins), m_uvAtMaxs(uvAtMaxs)
{

}

//-----------------------------------------------------------------------------------------------
void SpriteDefinition::GetUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs) const {
	out_uvAtMins = m_uvAtMins; out_uvAtMaxs = m_uvAtMaxs;
}

//-----------------------------------------------------------------------------------------------
AABB2 SpriteDefinition::GetUVs() const {
	return AABB2(m_uvAtMins, m_uvAtMaxs);
}

//-----------------------------------------------------------------------------------------------
SpriteSheet const& SpriteDefinition::GetSpriteSheet() const {
	return m_spriteSheet;
}

//-----------------------------------------------------------------------------------------------
Texture& SpriteDefinition::GetTexture() const {
	return m_spriteSheet.m_texture;
}

//-----------------------------------------------------------------------------------------------
float SpriteDefinition::GetAspect() const {
	Vec2 diff = m_uvAtMaxs - m_uvAtMins;
	return diff.x / diff.y;
}