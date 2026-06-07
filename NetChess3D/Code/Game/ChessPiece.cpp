#include "Game/ChessPiece.hpp"

#include <cctype>

ChessPiece::ChessPiece(PieceType pieceType, int ownerPlayerIdx, IntVec2 const& square)
	: m_pieceType(pieceType)
	, m_ownerPlayerIdx(ownerPlayerIdx)
	, m_square(square)
{
}

void ChessPiece::Render() const
{
}

PieceType ChessPiece::GetPieceType() const
{
	return m_pieceType;
}

int ChessPiece::GetOwnerPlayerIdx() const
{
	return m_ownerPlayerIdx;
}

IntVec2 ChessPiece::GetSquare() const
{
	return m_square;
}

char ChessPiece::GetDisplayLetter() const
{
	char letter = ChessPieceDefinition::GetDefinition(m_pieceType).GetLetter();
	if (m_ownerPlayerIdx == 1) {
		letter = static_cast<char>(std::tolower(static_cast<unsigned char>(letter)));
	}
	return letter;
}

void ChessPiece::SetSquare(IntVec2 const& square)
{
	m_square = square;
}
