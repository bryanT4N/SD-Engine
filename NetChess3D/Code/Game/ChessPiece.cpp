#include "Game/ChessPiece.hpp"

#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <cctype>

namespace
{
	constexpr float PIECE_ANIM_DURATION = 0.25f;
	constexpr float KNIGHT_HOP_HEIGHT = 0.6f;

	Vec3 GetSquareCenter(IntVec2 const& square)
	{
		return Vec3(static_cast<float>(square.x) + 0.5f, static_cast<float>(square.y) + 0.5f, 0.f);
	}
}

ChessPiece::ChessPiece(PieceType pieceType, int ownerPlayerIdx, IntVec2 const& square)
	: m_pieceType(pieceType)
	, m_ownerPlayerIdx(ownerPlayerIdx)
	, m_square(square)
{
}

void ChessPiece::Render() const
{
	if (g_engine == nullptr || g_engine->m_render == nullptr) {
		return;
	}
	if (m_ownerPlayerIdx < 0 || m_ownerPlayerIdx > 1) {
		return;
	}

	ChessPieceDefinition const& def = ChessPieceDefinition::GetDefinition(m_pieceType);
	VertexBuffer* vbo = def.m_vertexBuffersPerSide[m_ownerPlayerIdx];
	IndexBuffer* ibo = def.m_indexBuffersPerSide[m_ownerPlayerIdx];
	int indexCount = def.m_indexCountsPerSide[m_ownerPlayerIdx];
	if (vbo == nullptr || ibo == nullptr || indexCount == 0) {
		return;
	}

	Renderer* renderer = g_engine->m_render;
	Rgba8 tint = (m_ownerPlayerIdx == 0) ? PLAYER_TINT_GREEN : PLAYER_TINT_RED;

	Vec3 piecePosition = GetSquareCenter(m_square);
	if (m_isAnimating) {
		float t = m_animElapsed / PIECE_ANIM_DURATION;
		if (t > 1.f) {
			t = 1.f;
		}
		piecePosition = m_animStartPos + (m_animEndPos - m_animStartPos) * SmoothStep3(t);
		if (m_animIsHop) {
			piecePosition.z += KNIGHT_HOP_HEIGHT * 4.f * t * (1.f - t);
		}
	}
	Mat44 modelToWorld = Mat44::MakeTranslation3D(piecePosition);

	char const* diffusePath = (m_ownerPlayerIdx == 0)
		? "Data/Images/GreenPiece_d.jpg" : "Data/Images/RedPiece_d.jpg";
	char const* normalPath = (m_ownerPlayerIdx == 0)
		? "Data/Images/GreenPiece_n.jpg" : "Data/Images/RedPiece_n.jpg";

	renderer->SetModelCBO(modelToWorld, tint);
	renderer->BindTexture(renderer->CreateOrGetTextureFromFile(diffusePath), 0);
	renderer->BindTexture(renderer->CreateOrGetTextureFromFile(normalPath), 1);
	renderer->DrawIndexedVertexBuffer(vbo, ibo, static_cast<unsigned int>(indexCount));
}

void ChessPiece::StartMoveAnimation(IntVec2 const& fromSquare, IntVec2 const& toSquare, bool isHop)
{
	m_animStartPos = GetSquareCenter(fromSquare);
	m_animEndPos = GetSquareCenter(toSquare);
	m_animElapsed = 0.f;
	m_animIsHop = isHop;
	m_isAnimating = true;
}

void ChessPiece::UpdateAnimation(float deltaSeconds)
{
	if (!m_isAnimating) {
		return;
	}
	m_animElapsed += deltaSeconds;
	if (m_animElapsed >= PIECE_ANIM_DURATION) {
		m_isAnimating = false;
	}
}

char ChessPiece::GetDisplayLetter() const
{
	char letter = ChessPieceDefinition::GetDefinition(m_pieceType).m_letter;
	if (m_ownerPlayerIdx == 1) {
		letter = static_cast<char>(std::tolower(static_cast<unsigned char>(letter)));
	}
	return letter;
}
