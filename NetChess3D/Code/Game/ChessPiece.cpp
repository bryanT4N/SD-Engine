#include "Game/ChessPiece.hpp"

#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <cctype>

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

	Vec3 piecePosition(
		static_cast<float>(m_square.x) + 0.5f,
		static_cast<float>(m_square.y) + 0.5f,
		0.f);
	Mat44 modelToWorld = Mat44::MakeTranslation3D(piecePosition);

	renderer->SetModelCBO(modelToWorld, tint);
	renderer->BindTexture(renderer->CreateOrGetTextureFromFile("Data/Images/Bricks_d.png"), 0);
	renderer->BindTexture(renderer->CreateOrGetTextureFromFile("Data/Images/Bricks_n.png"), 1);
	renderer->DrawIndexedVertexBuffer(vbo, ibo, static_cast<unsigned int>(indexCount));
}

char ChessPiece::GetDisplayLetter() const
{
	char letter = ChessPieceDefinition::GetDefinition(m_pieceType).m_letter;
	if (m_ownerPlayerIdx == 1) {
		letter = static_cast<char>(std::tolower(static_cast<unsigned char>(letter)));
	}
	return letter;
}
